// ============================================================
// Ultimate Source Engine - Console System Implementation
// ============================================================

#include "stdafx.h"
#include "ConsoleSystem.h"
#include "ConsoleCommands.h"
#include "ConsoleVariables.h"
#include "Core/Engine.h"
#include "Core/Platform.h"
#include "Core/Window.h"
#include "Input/InputManager.h"
#include "Utility/StringUtils.h"
#include "Utility/Logger.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <regex>

#ifdef _WIN32
#include <windows.h>
#include <clipboard.h>
#endif

namespace USE {

    // -----------------------------------------------------------------
    // ConsoleSystem Implementation
    // -----------------------------------------------------------------

    ConsoleSystem::ConsoleSystem()
        : m_bInitialized(false)
        , m_bIsOpen(false)
        , m_animationTime(0.0f)
        , m_currentHeight(0.0f)
        , m_pRenderer(nullptr)
        , m_pWindow(nullptr)
        , m_pFont(nullptr)
        , m_pBackgroundTexture(nullptr)
        , m_scrollOffset(0)
        , m_cursorPosition(0)
        , m_selectionStart(0)
        , m_selectionEnd(0)
        , m_cursorBlinkTimer(0.0f)
        , m_cursorVisible(true)
        , m_historyIndex(-1)
        , m_autoCompleteIndex(-1)
    {
        m_colorScheme = ConsoleColorScheme::DefaultSourceStyle();
        m_messageFilter.set();
    }

    ConsoleSystem::~ConsoleSystem()
    {
        Shutdown();
    }

    bool ConsoleSystem::Initialize()
    {
        if (m_bInitialized) {
            USE_LOG_WARN("ConsoleSystem already initialized");
            return true;
        }

        USE_LOG_INFO("Initializing ConsoleSystem...");

        // Get engine subsystems
        Engine* engine = Engine::Get();
        if (!engine) {
            USE_LOG_ERROR("Engine not available for console initialization");
            return false;
        }

        m_pRenderer = engine->GetRenderer();
        m_pWindow = engine->GetWindow();

        if (!m_pRenderer || !m_pWindow) {
            USE_LOG_ERROR("Renderer or Window not available for console");
            return false;
        }

        // Load default font (engine should provide this)
        // m_pFont = engine->GetResourceManager()->LoadFont("console_font.ttf", m_settings.fontSize);

        // Register console commands and CVars
        ConsoleCommands::RegisterAll(this);
        ConsoleVariables::RegisterAll(this);

        // Load command history
        std::string historyPath = Platform::GetUserDirectory() + "/console_history.txt";
        std::ifstream historyFile(historyPath);
        if (historyFile.is_open()) {
            std::string line;
            while (std::getline(historyFile, line)) {
                if (!line.empty()) {
                    m_commandHistory.push_back(line);
                }
            }
            historyFile.close();
            USE_LOG_INFO("Loaded %zu commands from console history", m_commandHistory.size());
        }

        // Open log file if enabled
        if (m_settings.logToFile) {
            m_logFile.open(m_settings.logFilePath, std::ios::app);
            if (m_logFile.is_open()) {
                USE_LOG_INFO("Console logging to: %s", m_settings.logFilePath.c_str());
            }
        }

        m_bInitialized = true;
        USE_LOG_INFO("ConsoleSystem initialized successfully");

        ShowSplashScreen();

        return true;
    }

    void ConsoleSystem::Shutdown()
    {
        if (!m_bInitialized) return;

        USE_LOG_INFO("Shutting down ConsoleSystem...");

        // Save command history
        std::string historyPath = Platform::GetUserDirectory() + "/console_history.txt";
        std::ofstream historyFile(historyPath);
        if (historyFile.is_open()) {
            int count = 0;
            for (const auto& cmd : m_commandHistory) {
                historyFile << cmd << std::endl;
                if (++count >= m_settings.maxHistorySize) break;
            }
            historyFile.close();
            USE_LOG_INFO("Saved %d commands to console history", count);
        }

        // Close log file
        if (m_logFile.is_open()) {
            m_logFile.close();
        }

        // Clear CVars
        for (auto& pair : m_cvars) {
            delete pair.second;
        }
        m_cvars.clear();

        // Clear commands
        m_commands.clear();

        m_bInitialized = false;
        USE_LOG_INFO("ConsoleSystem shutdown complete");
    }

    void ConsoleSystem::Open()
    {
        if (!m_bInitialized) return;
        if (!m_bIsOpen) {
            m_bIsOpen = true;
            m_animationTime = 0.0f;
            m_historyIndex = -1;
            m_autoCompleteIndex = -1;
            m_autoCompleteSuggestions.clear();
            USE_LOG_DEBUG("Console opened");
        }
    }

    void ConsoleSystem::Close()
    {
        if (!m_bInitialized) return;
        if (m_bIsOpen) {
            m_bIsOpen = false;
            m_animationTime = 0.0f;
            USE_LOG_DEBUG("Console closed");
        }
    }

    void ConsoleSystem::Toggle()
    {
        if (m_bIsOpen) Close(); else Open();
    }

    void ConsoleSystem::Update(float deltaTime)
    {
        if (!m_bInitialized) return;

        // Animate console height
        if (m_settings.enableAnimations) {
            if (m_bIsOpen && m_currentHeight < m_settings.heightPercentage) {
                m_animationTime += deltaTime * m_settings.animationSpeed;
                m_currentHeight = std::min(m_settings.heightPercentage, m_animationTime);
            } else if (!m_bIsOpen && m_currentHeight > 0.0f) {
                m_animationTime -= deltaTime * m_settings.animationSpeed;
                m_currentHeight = std::max(0.0f, m_animationTime);
            }
        } else {
            m_currentHeight = m_bIsOpen ? m_settings.heightPercentage : 0.0f;
        }

        // Update cursor blink
        m_cursorBlinkTimer += deltaTime;
        if (m_cursorBlinkTimer >= 1.0f / m_settings.cursorBlinkRate) {
            m_cursorVisible = !m_cursorVisible;
            m_cursorBlinkTimer = 0.0f;
        }

        // Clamp scroll offset
        int maxMessages = static_cast<int>(m_messages.size());
        int maxScroll = std::max(0, maxMessages - m_settings.visibleLineCount);
        m_scrollOffset = std::clamp(m_scrollOffset, 0, maxScroll);
    }

    void ConsoleSystem::Render()
    {
        if (!m_bInitialized || m_currentHeight <= 0.001f) return;

        // Save render state (using immediate mode for simplicity)
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();

        int width = m_pWindow->GetWidth();
        int height = m_pWindow->GetHeight();
        glOrtho(0, width, height, 0, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        RenderBackground();
        RenderMessages();
        RenderInputLine();
        RenderScrollbar();
        RenderAutoComplete();

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopAttrib();
    }

    void ConsoleSystem::RenderBackground()
    {
        int width = m_pWindow->GetWidth();
        int height = m_pWindow->GetHeight();
        int consoleHeight = static_cast<int>(height * m_currentHeight);

        glBegin(GL_QUADS);
        glColor4f(m_colorScheme.background.r, m_colorScheme.background.g,
                 m_colorScheme.background.b, m_colorScheme.background.a);
        glVertex2i(0, 0);
        glVertex2i(width, 0);
        glVertex2i(width, consoleHeight);
        glVertex2i(0, consoleHeight);
        glEnd();

        // Draw border
        glBegin(GL_LINES);
        glColor4f(m_colorScheme.border.r, m_colorScheme.border.g,
                 m_colorScheme.border.b, m_colorScheme.border.a);
        glVertex2i(0, consoleHeight);
        glVertex2i(width, consoleHeight);
        glEnd();
    }

    void ConsoleSystem::RenderMessages()
    {
        // Simplified: just a placeholder for actual text rendering
        // In a full implementation, you'd use a font renderer.
    }

    void ConsoleSystem::RenderInputLine()
    {
        int width = m_pWindow->GetWidth();
        int height = m_pWindow->GetHeight();
        int consoleHeight = static_cast<int>(height * m_currentHeight);
        int inputHeight = static_cast<int>(m_settings.fontSize * m_settings.lineHeight + 8);
        int inputY = consoleHeight - inputHeight;

        // Input background
        glBegin(GL_QUADS);
        glColor4f(m_colorScheme.inputBackground.r, m_colorScheme.inputBackground.g,
                 m_colorScheme.inputBackground.b, m_colorScheme.inputBackground.a);
        glVertex2i(0, inputY);
        glVertex2i(width, inputY);
        glVertex2i(width, consoleHeight);
        glVertex2i(0, consoleHeight);
        glEnd();

        // Prompt
        // Render text using m_pFont (not shown)
    }

    void ConsoleSystem::RenderCursor()
    {
        if (!m_cursorVisible) return;
        // Draw cursor as a small vertical line (simplified)
        int width = m_pWindow->GetWidth();
        int height = m_pWindow->GetHeight();
        int consoleHeight = static_cast<int>(height * m_currentHeight);
        int inputHeight = static_cast<int>(m_settings.fontSize * m_settings.lineHeight + 8);
        int inputY = consoleHeight - inputHeight;
        int cursorX = 5 + static_cast<int>(m_cursorPosition * m_settings.fontSize * 0.5f); // rough estimate

        glBegin(GL_QUADS);
        glColor4f(m_colorScheme.cursor.r, m_colorScheme.cursor.g,
                 m_colorScheme.cursor.b, m_colorScheme.cursor.a);
        glVertex2i(cursorX, inputY + 2);
        glVertex2i(cursorX + 2, inputY + 2);
        glVertex2i(cursorX + 2, consoleHeight - 2);
        glVertex2i(cursorX, consoleHeight - 2);
        glEnd();
    }

    void ConsoleSystem::RenderScrollbar()
    {
        // Simplified scrollbar rendering
    }

    void ConsoleSystem::RenderAutoComplete()
    {
        if (!m_settings.autoComplete || m_autoCompleteSuggestions.empty()) return;
        // Draw suggestion box (placeholder)
    }

    void ConsoleSystem::Print(const std::string& text, ConsoleMessageType type, const std::string& module)
    {
        if (!m_bInitialized) return;

        // Check for duplicates
        if (!m_messages.empty()) {
            auto& lastMsg = m_messages.back();
            if (lastMsg.text == text && lastMsg.type == type && lastMsg.module == module) {
                lastMsg.repeatCount++;
                return;
            }
        }

        ConsoleMessage msg(type, text, module);
        msg.timestamp = Platform::GetMicroseconds() / 1000000.0; // seconds
        m_messages.push_back(msg);

        while (m_messages.size() > static_cast<size_t>(m_settings.maxMessageCount)) {
            m_messages.pop_front();
        }

        if (m_settings.logToFile) {
            LogToFile(msg);
        }

        // Also forward to engine logger
        switch (type) {
            case ConsoleMessageType::Warning: USE_LOG_WARN("%s", text.c_str()); break;
            case ConsoleMessageType::Error:   USE_LOG_ERROR("%s", text.c_str()); break;
            case ConsoleMessageType::Debug:   USE_LOG_DEBUG("%s", text.c_str()); break;
            default:                          USE_LOG_INFO("%s", text.c_str()); break;
        }
    }

    bool ConsoleSystem::Execute(const std::string& command)
    {
        if (!m_bInitialized) return false;

        std::string trimmed = StringUtils::Trim(command);
        if (trimmed.empty()) return true;

        AddToHistory(trimmed);
        Print("> " + trimmed, ConsoleMessageType::Command);

        std::vector<std::string> tokens = StringUtils::Split(trimmed, ' ');
        if (tokens.empty()) return false;

        std::string cmdName = StringUtils::ToLower(tokens[0]);
        std::vector<std::string> args(tokens.begin() + 1, tokens.end());

        // CVar assignment
        size_t equalsPos = cmdName.find('=');
        if (equalsPos != std::string::npos) {
            std::string varName = cmdName.substr(0, equalsPos);
            std::string value = cmdName.substr(equalsPos + 1);
            auto it = m_cvars.find(varName);
            if (it != m_cvars.end()) {
                if (it->second->HasFlag(CVarFlags::ReadOnly)) {
                    PrintError("CVar '" + varName + "' is read-only");
                    return false;
                }
                it->second->SetString(value);
                Print(varName + " = " + it->second->GetString(), ConsoleMessageType::Output);
                return true;
            }
        }

        // Command dispatch
        auto cmdIt = m_commands.find(cmdName);
        if (cmdIt != m_commands.end()) {
            const auto& cmdInfo = cmdIt->second;
            if (cmdInfo.isCheat && !m_settings.developerMode) {
                PrintError("Command '" + cmdName + "' is cheat protected");
                return false;
            }
            if (cmdInfo.isDevOnly && !m_settings.developerMode) {
                PrintError("Command '" + cmdName + "' is developer only");
                return false;
            }
            if (static_cast<int>(args.size()) < cmdInfo.minArgs) {
                PrintError("Command '" + cmdName + "' requires at least " + std::to_string(cmdInfo.minArgs) + " arguments");
                if (!cmdInfo.usage.empty()) Print("Usage: " + cmdInfo.usage, ConsoleMessageType::Info);
                return false;
            }
            if (cmdInfo.maxArgs >= 0 && static_cast<int>(args.size()) > cmdInfo.maxArgs) {
                PrintError("Command '" + cmdName + "' accepts at most " + std::to_string(cmdInfo.maxArgs) + " arguments");
                if (!cmdInfo.usage.empty()) Print("Usage: " + cmdInfo.usage, ConsoleMessageType::Info);
                return false;
            }
            try {
                cmdInfo.func(args);
                return true;
            } catch (const std::exception& e) {
                PrintError("Command execution failed: " + std::string(e.what()));
                return false;
            }
        }

        // CVar query
        auto cvarIt = m_cvars.find(cmdName);
        if (cvarIt != m_cvars.end()) {
            Print(cmdName + " = " + cvarIt->second->GetString() + "  [" + cvarIt->second->GetDescription() + "]", ConsoleMessageType::Output);
            return true;
        }

        PrintError("Unknown command: '" + cmdName + "'");
        return false;
    }

    bool ConsoleSystem::ExecuteFile(const std::string& filename)
    {
        // Use engine's file system
        auto file = Engine::Get()->GetFileSystem()->OpenFile(filename, FILE_READ);
        if (!file) {
            PrintError("Could not open file: " + filename);
            return false;
        }

        Print("Executing script: " + filename, ConsoleMessageType::System);

        std::string line;
        int lineNum = 0;
        int successCount = 0;
        int failCount = 0;

        while (file->ReadLine(line)) {
            lineNum++;
            std::string trimmed = StringUtils::Trim(line);
            if (trimmed.empty() || trimmed[0] == ';' || trimmed[0] == '#') continue;

            // Remove trailing comments
            size_t commentPos = trimmed.find(';');
            if (commentPos != std::string::npos) trimmed = trimmed.substr(0, commentPos);
            commentPos = trimmed.find('#');
            if (commentPos != std::string::npos) trimmed = trimmed.substr(0, commentPos);

            trimmed = StringUtils::Trim(trimmed);
            if (trimmed.empty()) continue;

            if (Execute(trimmed)) {
                successCount++;
            } else {
                failCount++;
                PrintError("  Line " + std::to_string(lineNum) + ": " + trimmed);
            }
        }

        Print("Script execution complete: " + std::to_string(successCount) + " succeeded, " + std::to_string(failCount) + " failed",
              failCount > 0 ? ConsoleMessageType::Warning : ConsoleMessageType::Success);

        return failCount == 0;
    }

    void ConsoleSystem::Clear()
    {
        m_messages.clear();
        m_scrollOffset = 0;
        Print("Console cleared", ConsoleMessageType::System);
    }

    void ConsoleSystem::RegisterCommand(const std::string& name, ConsoleCommandFunc func,
                                      const std::string& description, const std::string& usage,
                                      int minArgs, int maxArgs, bool isCheat, bool isDevOnly)
    {
        std::string lowerName = StringUtils::ToLower(name);
        ConsoleCommandInfo info;
        info.name = lowerName;
        info.description = description;
        info.usage = usage.empty() ? lowerName : usage;
        info.func = func;
        info.minArgs = minArgs;
        info.maxArgs = maxArgs;
        info.isCheat = isCheat;
        info.isDevOnly = isDevOnly;
        m_commands[lowerName] = info;
    }

    void ConsoleSystem::UnregisterCommand(const std::string& name)
    {
        std::string lowerName = StringUtils::ToLower(name);
        m_commands.erase(lowerName);
    }

    bool ConsoleSystem::HasCommand(const std::string& name) const
    {
        std::string lowerName = StringUtils::ToLower(name);
        return m_commands.find(lowerName) != m_commands.end();
    }

    template<typename T>
    ConsoleVariable<T>* ConsoleSystem::RegisterCVar(const std::string& name, const T& defaultValue,
                                                  const std::string& description, CVarFlags flags)
    {
        std::string lowerName = StringUtils::ToLower(name);
        if (m_cvars.find(lowerName) != m_cvars.end()) {
            USE_LOG_WARN("CVar '%s' already registered", lowerName.c_str());
            return nullptr;
        }
        ConsoleVariable<T>* cvar = new ConsoleVariable<T>(lowerName, defaultValue, description, flags);
        m_cvars[lowerName] = cvar;
        return cvar;
    }

    // Template instantiations (would be in .cpp)
    template ConsoleVariable<int>* ConsoleSystem::RegisterCVar<int>(const std::string&, const int&, const std::string&, CVarFlags);
    template ConsoleVariable<float>* ConsoleSystem::RegisterCVar<float>(const std::string&, const float&, const std::string&, CVarFlags);
    template ConsoleVariable<bool>* ConsoleSystem::RegisterCVar<bool>(const std::string&, const bool&, const std::string&, CVarFlags);
    template ConsoleVariable<std::string>* ConsoleSystem::RegisterCVar<std::string>(const std::string&, const std::string&, const std::string&, CVarFlags);

    ConsoleVariableBase* ConsoleSystem::GetCVar(const std::string& name)
    {
        std::string lowerName = StringUtils::ToLower(name);
        auto it = m_cvars.find(lowerName);
        return it != m_cvars.end() ? it->second : nullptr;
    }

    void ConsoleSystem::UnregisterCVar(const std::string& name)
    {
        std::string lowerName = StringUtils::ToLower(name);
        auto it = m_cvars.find(lowerName);
        if (it != m_cvars.end()) {
            delete it->second;
            m_cvars.erase(it);
        }
    }

    void ConsoleSystem::ProcessKeyEvent(int key, int scancode, int action, int mods)
    {
        if (!m_bIsOpen || !m_bInitialized) return;

        if (action == SDL_PRESSED || action == SDL_REPEATED) {
            switch (key) {
                case SDLK_RETURN: case SDLK_KP_ENTER: ProcessInput(); break;
                case SDLK_BACKSPACE: HandleBackspace(mods); break;
                case SDLK_DELETE: if (m_cursorPosition < static_cast<int>(m_inputBuffer.length())) m_inputBuffer.erase(m_cursorPosition, 1); UpdateAutoComplete(); break;
                case SDLK_LEFT: if (m_cursorPosition > 0) m_cursorPosition--; break;
                case SDLK_RIGHT: if (m_cursorPosition < static_cast<int>(m_inputBuffer.length())) m_cursorPosition++; break;
                case SDLK_HOME: m_cursorPosition = 0; break;
                case SDLK_END: m_cursorPosition = static_cast<int>(m_inputBuffer.length()); break;
                case SDLK_UP: m_inputBuffer = GetPreviousCommand(); m_cursorPosition = static_cast<int>(m_inputBuffer.length()); UpdateAutoComplete(); break;
                case SDLK_DOWN: m_inputBuffer = GetNextCommand(); m_cursorPosition = static_cast<int>(m_inputBuffer.length()); UpdateAutoComplete(); break;
                case SDLK_TAB: HandleTab(); break;
                case SDLK_ESCAPE: Close(); break;
                // Ctrl+C, Ctrl+V, etc. can be added
            }
        }
    }

    void ConsoleSystem::HandleBackspace(int mods)
    {
        if (mods & KMOD_CTRL) {
            // Delete word
            size_t wordStart = m_inputBuffer.find_last_of(" \t", m_cursorPosition - 1);
            if (wordStart == std::string::npos) wordStart = 0; else wordStart++;
            m_inputBuffer.erase(wordStart, m_cursorPosition - wordStart);
            m_cursorPosition = static_cast<int>(wordStart);
        } else if (m_cursorPosition > 0) {
            m_inputBuffer.erase(m_cursorPosition - 1, 1);
            m_cursorPosition--;
        }
        UpdateAutoComplete();
    }

    void ConsoleSystem::HandleTab()
    {
        if (!m_settings.autoComplete || m_autoCompleteSuggestions.empty()) return;
        if (m_autoCompleteIndex < 0) m_autoCompleteIndex = 0;
        else m_autoCompleteIndex = (m_autoCompleteIndex + 1) % static_cast<int>(m_autoCompleteSuggestions.size());
        m_inputBuffer = m_autoCompleteSuggestions[m_autoCompleteIndex];
        m_cursorPosition = static_cast<int>(m_inputBuffer.length());
    }

    void ConsoleSystem::ProcessCharEvent(unsigned int codepoint)
    {
        if (!m_bIsOpen || !m_bInitialized) return;
        if (codepoint >= 32 && codepoint < 128) {
            DeleteSelection();
            m_inputBuffer.insert(m_cursorPosition, 1, static_cast<char>(codepoint));
            m_cursorPosition++;
            m_selectionStart = m_selectionEnd = m_cursorPosition;
            UpdateAutoComplete();
        }
    }

    void ConsoleSystem::ProcessMouseScroll(float yoffset)
    {
        if (!m_bIsOpen || !m_bInitialized) return;
        m_scrollOffset -= static_cast<int>(yoffset * m_settings.scrollSpeed);
        int maxMessages = static_cast<int>(GetFilteredMessages().size());
        int maxScroll = std::max(0, maxMessages - m_settings.visibleLineCount);
        m_scrollOffset = std::clamp(m_scrollOffset, 0, maxScroll);
    }

    void ConsoleSystem::SetSearchFilter(const std::string& filter)
    {
        m_searchFilter = StringUtils::ToLower(filter);
        m_scrollOffset = 0;
    }

    void ConsoleSystem::ClearSearchFilter()
    {
        m_searchFilter.clear();
        m_scrollOffset = 0;
    }

    void ConsoleSystem::SetMessageFilter(ConsoleMessageType type, bool enabled)
    {
        m_messageFilter.set(static_cast<int>(type), enabled);
        m_scrollOffset = 0;
    }

    std::deque<ConsoleMessage> ConsoleSystem::GetFilteredMessages() const
    {
        std::deque<ConsoleMessage> filtered;
        for (const auto& msg : m_messages) {
            if (!m_messageFilter.test(static_cast<int>(msg.type))) continue;
            if (!m_searchFilter.empty()) {
                std::string lowerText = StringUtils::ToLower(msg.text);
                if (lowerText.find(m_searchFilter) == std::string::npos) continue;
            }
            filtered.push_back(msg);
        }
        return filtered;
    }

    std::vector<std::string> ConsoleSystem::GetAutoCompleteSuggestions(const std::string& input)
    {
        std::vector<std::string> suggestions;
        if (input.empty()) return suggestions;
        std::string lowerInput = StringUtils::ToLower(input);

        for (const auto& pair : m_commands) {
            if (pair.first.find(lowerInput) == 0) suggestions.push_back(pair.first);
        }
        for (const auto& pair : m_cvars) {
            if (pair.first.find(lowerInput) == 0) suggestions.push_back(pair.first);
        }
        std::sort(suggestions.begin(), suggestions.end());
        if (static_cast<int>(suggestions.size()) > m_settings.maxAutoCompleteLines)
            suggestions.resize(m_settings.maxAutoCompleteLines);
        return suggestions;
    }

    void ConsoleSystem::UpdateAutoComplete()
    {
        if (!m_settings.autoComplete) {
            m_autoCompleteSuggestions.clear();
            m_autoCompleteIndex = -1;
            return;
        }
        // Get current word
        size_t lastSpace = m_inputBuffer.find_last_of(" \t", m_cursorPosition - 1);
        std::string currentWord;
        if (lastSpace == std::string::npos)
            currentWord = m_inputBuffer.substr(0, m_cursorPosition);
        else
            currentWord = m_inputBuffer.substr(lastSpace + 1, m_cursorPosition - lastSpace - 1);

        m_autoCompleteSuggestions = GetAutoCompleteSuggestions(currentWord);
        m_autoCompleteBase = currentWord;
        m_autoCompleteIndex = m_autoCompleteSuggestions.empty() ? -1 : 0;
    }

    void ConsoleSystem::ProcessInput()
    {
        if (m_inputBuffer.empty()) return;
        Execute(m_inputBuffer);
        m_inputBuffer.clear();
        m_cursorPosition = 0;
        m_selectionStart = m_selectionEnd = 0;
        m_historyIndex = -1;
        UpdateAutoComplete();
    }

    void ConsoleSystem::DeleteSelection()
    {
        if (m_selectionStart == m_selectionEnd) return;
        int start = std::min(m_selectionStart, m_selectionEnd);
        int end = std::max(m_selectionStart, m_selectionEnd);
        m_inputBuffer.erase(start, end - start);
        m_cursorPosition = start;
        m_selectionStart = m_selectionEnd = m_cursorPosition;
    }

    void ConsoleSystem::CopyToClipboard()
    {
        if (m_selectionStart == m_selectionEnd) return;
        int start = std::min(m_selectionStart, m_selectionEnd);
        int end = std::max(m_selectionStart, m_selectionEnd);
        std::string selected = m_inputBuffer.substr(start, end - start);
#ifdef _WIN32
        if (OpenClipboard(nullptr)) {
            EmptyClipboard();
            HGLOBAL hGlob = GlobalAlloc(GMEM_FIXED, selected.length() + 1);
            memcpy(hGlob, selected.c_str(), selected.length() + 1);
            SetClipboardData(CF_TEXT, hGlob);
            CloseClipboard();
            GlobalFree(hGlob);
        }
#endif
    }

    void ConsoleSystem::PasteFromClipboard()
    {
#ifdef _WIN32
        if (OpenClipboard(nullptr)) {
            HANDLE hData = GetClipboardData(CF_TEXT);
            if (hData) {
                char* pszText = static_cast<char*>(GlobalLock(hData));
                if (pszText) {
                    DeleteSelection();
                    std::string text(pszText);
                    m_inputBuffer.insert(m_cursorPosition, text);
                    m_cursorPosition += static_cast<int>(text.length());
                    GlobalUnlock(hData);
                }
            }
            CloseClipboard();
        }
#endif
    }

    void ConsoleSystem::AddToHistory(const std::string& command)
    {
        if (!m_commandHistory.empty() && m_commandHistory.back() == command) return;
        m_commandHistory.push_back(command);
        while (m_commandHistory.size() > static_cast<size_t>(m_settings.maxHistorySize))
            m_commandHistory.pop_front();
        m_historyIndex = -1;
    }

    std::string ConsoleSystem::GetPreviousCommand()
    {
        if (m_commandHistory.empty()) return m_inputBuffer;
        if (m_historyIndex < 0)
            m_historyIndex = static_cast<int>(m_commandHistory.size()) - 1;
        else if (m_historyIndex > 0)
            m_historyIndex--;
        return m_commandHistory[m_historyIndex];
    }

    std::string ConsoleSystem::GetNextCommand()
    {
        if (m_commandHistory.empty()) return m_inputBuffer;
        if (m_historyIndex >= 0) {
            m_historyIndex++;
            if (m_historyIndex >= static_cast<int>(m_commandHistory.size())) {
                m_historyIndex = -1;
                return "";
            }
            return m_commandHistory[m_historyIndex];
        }
        return m_inputBuffer;
    }

    void ConsoleSystem::ShowSplashScreen()
    {
        Print("==================================================", ConsoleMessageType::System);
        Print("    Ultimate Source Engine v1.0.0 - Console", ConsoleMessageType::System);
        Print("==================================================", ConsoleMessageType::System);
        Print("Type 'help' for commands.", ConsoleMessageType::Info);
        Print("", ConsoleMessageType::System);
    }

    void ConsoleSystem::LogToFile(const ConsoleMessage& message)
    {
        if (!m_logFile.is_open()) return;
        char timestamp[32];
        time_t now = time(nullptr);
        tm* tm_info = localtime(&now);
        strftime(timestamp, 32, "[%Y-%m-%d %H:%M:%S] ", tm_info);
        m_logFile << timestamp << message.prefix << message.text;
        if (message.repeatCount > 1) m_logFile << " (x" << message.repeatCount << ")";
        m_logFile << std::endl;
        m_logFile.flush();
    }

} // namespace USE