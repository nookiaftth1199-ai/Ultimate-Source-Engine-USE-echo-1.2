// ============================================================
// Ultimate Source Engine - Console System
// Version: 1.0.0
// ============================================================

#pragma once

#include "Core/Engine.h"
#include "Core/Singleton.h"
#include "Math/Color.h"
#include "Math/Vector2.h"
#include "Input/InputManager.h"
#include "Renderer/RenderSystem.h"
#include "Utility/Logger.h"
#include <functional>
#include <vector>
#include <deque>
#include <map>
#include <bitset>
#include <regex>

namespace USE {

    // Forward declarations
    class IWindow;
    class IRenderDevice;
    class Font;
    class Texture2D;

    // -----------------------------------------------------------------
    // Console Message Types
    // -----------------------------------------------------------------
    enum class ConsoleMessageType {
        Info = 0,
        Warning,
        Error,
        Command,
        Output,
        Debug,
        Success,
        System,
        Network,
        Physics,
        Audio,
        Renderer,
        AI,
        Count
    };

    // -----------------------------------------------------------------
    // Console Message Structure
    // -----------------------------------------------------------------
    struct ConsoleMessage {
        ConsoleMessageType type;
        std::string text;
        std::string prefix;
        double timestamp;
        int repeatCount;
        std::string module;

        ConsoleMessage() : type(ConsoleMessageType::Info), timestamp(0.0), repeatCount(1) {}
        ConsoleMessage(ConsoleMessageType t, const std::string& txt,
                      const std::string& mod = "", const std::string& pre = "")
            : type(t), text(txt), prefix(pre), timestamp(0.0), repeatCount(1), module(mod) {}
    };

    // -----------------------------------------------------------------
    // Console Command Function
    // -----------------------------------------------------------------
    using ConsoleCommandFunc = std::function<void(const std::vector<std::string>&)>;

    // -----------------------------------------------------------------
    // Console Command Info
    // -----------------------------------------------------------------
    struct ConsoleCommandInfo {
        std::string name;
        std::string description;
        std::string usage;
        ConsoleCommandFunc func;
        int minArgs;
        int maxArgs;
        bool isCheat;
        bool isDevOnly;

        ConsoleCommandInfo() : minArgs(0), maxArgs(-1), isCheat(false), isDevOnly(false) {}
    };

    // -----------------------------------------------------------------
    // Console Color Scheme
    // -----------------------------------------------------------------
    struct ConsoleColorScheme {
        Color background;
        Color border;
        Color inputBackground;
        Color inputText;
        Color cursor;
        Color selection;
        Color scrollbar;
        Color scrollbarThumb;

        Color messageColors[static_cast<int>(ConsoleMessageType::Count)];

        static ConsoleColorScheme DefaultSourceStyle() {
            ConsoleColorScheme scheme;
            scheme.background = Color(0.0f, 0.0f, 0.0f, 0.85f);
            scheme.border = Color(0.3f, 0.3f, 0.3f, 1.0f);
            scheme.inputBackground = Color(0.1f, 0.1f, 0.1f, 0.95f);
            scheme.inputText = Color(1.0f, 1.0f, 1.0f, 1.0f);
            scheme.cursor = Color(1.0f, 1.0f, 1.0f, 0.8f);
            scheme.selection = Color(0.2f, 0.4f, 0.8f, 0.5f);
            scheme.scrollbar = Color(0.2f, 0.2f, 0.2f, 0.8f);
            scheme.scrollbarThumb = Color(0.5f, 0.5f, 0.5f, 0.9f);

            scheme.messageColors[static_cast<int>(ConsoleMessageType::Info)] = Color(1.0f, 1.0f, 1.0f, 1.0f);
            scheme.messageColors[static_cast<int>(ConsoleMessageType::Warning)] = Color(1.0f, 1.0f, 0.0f, 1.0f);
            scheme.messageColors[static_cast<int>(ConsoleMessageType::Error)] = Color(1.0f, 0.3f, 0.3f, 1.0f);
            scheme.messageColors[static_cast<int>(ConsoleMessageType::Command)] = Color(0.4f, 1.0f, 0.4f, 1.0f);
            scheme.messageColors[static_cast<int>(ConsoleMessageType::Output)] = Color(0.7f, 0.7f, 1.0f, 1.0f);
            scheme.messageColors[static_cast<int>(ConsoleMessageType::Debug)] = Color(0.5f, 0.5f, 0.5f, 1.0f);
            scheme.messageColors[static_cast<int>(ConsoleMessageType::Success)] = Color(0.0f, 1.0f, 0.0f, 1.0f);
            scheme.messageColors[static_cast<int>(ConsoleMessageType::System)] = Color(1.0f, 1.0f, 1.0f, 1.0f);
            scheme.messageColors[static_cast<int>(ConsoleMessageType::Network)] = Color(0.3f, 0.6f, 1.0f, 1.0f);
            scheme.messageColors[static_cast<int>(ConsoleMessageType::Physics)] = Color(0.8f, 0.5f, 1.0f, 1.0f);
            scheme.messageColors[static_cast<int>(ConsoleMessageType::Audio)] = Color(1.0f, 0.6f, 0.2f, 1.0f);
            scheme.messageColors[static_cast<int>(ConsoleMessageType::Renderer)] = Color(1.0f, 0.4f, 0.8f, 1.0f);
            scheme.messageColors[static_cast<int>(ConsoleMessageType::AI)] = Color(0.2f, 0.9f, 0.9f, 1.0f);

            return scheme;
        }
    };

    // -----------------------------------------------------------------
    // Console Settings
    // -----------------------------------------------------------------
    struct ConsoleSettings {
        float heightPercentage;
        float animationSpeed;
        int maxHistorySize;
        int maxMessageCount;
        int visibleLineCount;
        float fontSize;
        float lineHeight;
        bool showTimestamps;
        bool showModule;
        bool enableSound;
        bool enableAnimations;
        bool wordWrap;
        bool autoComplete;
        bool developerMode;
        bool logToFile;
        std::string logFilePath;
        int maxAutoCompleteLines;
        float scrollSpeed;
        float cursorBlinkRate;

        ConsoleSettings() {
            heightPercentage = 0.5f;
            animationSpeed = 8.0f;
            maxHistorySize = 100;
            maxMessageCount = 1000;
            visibleLineCount = 20;
            fontSize = 16.0f;
            lineHeight = 1.2f;
            showTimestamps = false;
            showModule = false;
            enableSound = true;
            enableAnimations = true;
            wordWrap = true;
            autoComplete = true;
            developerMode = false;
            logToFile = false;
            logFilePath = "console.log";
            maxAutoCompleteLines = 10;
            scrollSpeed = 1.0f;
            cursorBlinkRate = 2.0f;
        }
    };

    // -----------------------------------------------------------------
    // Main Console System
    // -----------------------------------------------------------------
    class ConsoleSystem : public Singleton<ConsoleSystem> {
    public:
        ConsoleSystem();
        virtual ~ConsoleSystem();

        bool Initialize();
        void Shutdown();

        void Open();
        void Close();
        void Toggle();
        bool IsOpen() const { return m_bIsOpen; }
        bool IsInitialized() const { return m_bInitialized; }

        void Update(float deltaTime);
        void Render();

        void Print(const std::string& text, ConsoleMessageType type = ConsoleMessageType::Info,
                  const std::string& module = "");
        void PrintInfo(const std::string& text) { Print(text, ConsoleMessageType::Info); }
        void PrintWarning(const std::string& text) { Print(text, ConsoleMessageType::Warning); }
        void PrintError(const std::string& text) { Print(text, ConsoleMessageType::Error); }
        void PrintDebug(const std::string& text) {
            if (m_settings.developerMode)
                Print(text, ConsoleMessageType::Debug);
        }
        void PrintSuccess(const std::string& text) { Print(text, ConsoleMessageType::Success); }
        void PrintSystem(const std::string& text) { Print(text, ConsoleMessageType::System); }

        bool Execute(const std::string& command);
        bool ExecuteFile(const std::string& filename);
        void Clear();

        void RegisterCommand(const std::string& name, ConsoleCommandFunc func,
                           const std::string& description = "",
                           const std::string& usage = "",
                           int minArgs = 0, int maxArgs = -1,
                           bool isCheat = false, bool isDevOnly = false);

        void UnregisterCommand(const std::string& name);
        bool HasCommand(const std::string& name) const;

        template<typename T>
        ConsoleVariable<T>* RegisterCVar(const std::string& name, const T& defaultValue,
                                       const std::string& description = "",
                                       CVarFlags flags = CVarFlags::None);

        ConsoleVariableBase* GetCVar(const std::string& name);
        void UnregisterCVar(const std::string& name);

        void ProcessKeyEvent(int key, int scancode, int action, int mods);
        void ProcessCharEvent(unsigned int codepoint);
        void ProcessMouseScroll(float yoffset);

        void SetColorScheme(const ConsoleColorScheme& scheme) { m_colorScheme = scheme; }
        void SetSettings(const ConsoleSettings& settings) { m_settings = settings; }
        ConsoleSettings& GetSettings() { return m_settings; }

        void SetSearchFilter(const std::string& filter);
        void ClearSearchFilter();
        void SetMessageFilter(ConsoleMessageType type, bool enabled);

        std::vector<std::string> GetAutoCompleteSuggestions(const std::string& input);
        std::vector<std::string> GetCommandHistory(int count = -1) const;

        const std::deque<ConsoleMessage>& GetMessages() const { return m_messages; }
        std::deque<ConsoleMessage> GetFilteredMessages() const;

        void SetFont(Font* pFont) { m_pFont = pFont; }
        void SetBackgroundTexture(Texture2D* pTexture) { m_pBackgroundTexture = pTexture; }

        void AddToHistory(const std::string& command);
        std::string GetPreviousCommand();
        std::string GetNextCommand();

        void ShowSplashScreen();

    private:
        bool m_bInitialized;
        bool m_bIsOpen;
        float m_animationTime;
        float m_currentHeight;

        IRenderDevice* m_pRenderer;
        IWindow* m_pWindow;
        Font* m_pFont;
        Texture2D* m_pBackgroundTexture;

        ConsoleSettings m_settings;
        ConsoleColorScheme m_colorScheme;

        std::deque<ConsoleMessage> m_messages;
        int m_scrollOffset;

        std::string m_inputBuffer;
        int m_cursorPosition;
        int m_selectionStart;
        int m_selectionEnd;
        float m_cursorBlinkTimer;
        bool m_cursorVisible;

        std::deque<std::string> m_commandHistory;
        int m_historyIndex;

        std::map<std::string, ConsoleCommandInfo> m_commands;
        std::map<std::string, ConsoleVariableBase*> m_cvars;

        std::vector<std::string> m_autoCompleteSuggestions;
        int m_autoCompleteIndex;
        std::string m_autoCompleteBase;

        std::string m_searchFilter;
        std::bitset<static_cast<int>(ConsoleMessageType::Count)> m_messageFilter;

        std::ofstream m_logFile;

        void RenderBackground();
        void RenderMessages();
        void RenderInputLine();
        void RenderCursor();
        void RenderSelection();
        void RenderScrollbar();
        void RenderAutoComplete();

        void UpdateAutoComplete();
        void ProcessInput();
        void DeleteSelection();
        void CopyToClipboard();
        void PasteFromClipboard();

        void LogToFile(const ConsoleMessage& message);

        friend class ConsoleCommands;
        friend class ConsoleVariables;
    };

    // Global console accessor
    #define USE_CONSOLE ConsoleSystem::Get()

} // namespace USE