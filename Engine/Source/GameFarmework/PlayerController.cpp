// ============================================================
// Ultimate Source Engine - Player Controller Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Entity/Components/CameraComponent.h"
#include "Input/InputManager.h"
#include "Core/Engine.h"
#include "Core/Logger.h"

namespace USE {

    PlayerController::PlayerController()
        : m_pawn(nullptr)
        , m_camera(nullptr)
        , m_moveInput(0,0)
        , m_lookInput(0,0)
        , m_jumpPressed(false)
        , m_firePressed(false)
    {
    }

    PlayerController::~PlayerController()
    {
        Unpossess();
    }

    void PlayerController::Possess(Pawn* pawn)
    {
        if (m_pawn == pawn) return;
        Unpossess();
        m_pawn = pawn;
        if (m_pawn) {
            // Optionally attach camera to pawn if not already set
            if (m_camera) {
                // camera can be independent, but we may want to attach to pawn's transform
                // For now, we assume camera is separate or part of pawn.
            }
            USE_LOG_INFO("PlayerController possessing pawn.");
        }
    }

    void PlayerController::Unpossess()
    {
        if (m_pawn) {
            // Clean up
            m_pawn = nullptr;
        }
    }

    void PlayerController::SetupInputBindings(InputManager* inputManager)
    {
        if (!inputManager) return;

        // Bind movement actions
        inputManager->BindAxis("MoveForward", GamepadAxis::LeftY, [this](float value) {
            MoveForward(value);
        });
        inputManager->BindAxis("MoveRight", GamepadAxis::LeftX, [this](float value) {
            MoveRight(value);
        });
        inputManager->BindAxis("LookHorizontal", GamepadAxis::RightX, [this](float value) {
            LookHorizontal(value);
        });
        inputManager->BindAxis("LookVertical", GamepadAxis::RightY, [this](float value) {
            LookVertical(value);
        });

        // Keyboard bindings
        inputManager->BindAction("Jump", SDL_SCANCODE_SPACE, [this](float value) {
            if (value > 0.5f) Jump();
        });
        inputManager->BindAction("Fire", SDL_SCANCODE_LCTRL, [this](float value) {
            if (value > 0.5f) Fire();
        });
        // Mouse look is handled by relative mouse motion; we can bind directly
        // For mouse, we can use InputManager's mouse delta via an axis binding.
        // This is not directly in the current InputManager, but we can add later.
    }

    void PlayerController::Update(float deltaTime)
    {
        // Process input (movement and look) and apply to pawn
        if (m_pawn) {
            ApplyMovement(deltaTime);
            ApplyLook(deltaTime);
        }
    }

    void PlayerController::MoveForward(float value)
    {
        m_moveInput.x = value;
    }

    void PlayerController::MoveRight(float value)
    {
        m_moveInput.y = value;
    }

    void PlayerController::MoveUp(float value)
    {
        // For flying/vertical movement
    }

    void PlayerController::LookHorizontal(float value)
    {
        m_lookInput.x = value;
    }

    void PlayerController::LookVertical(float value)
    {
        m_lookInput.y = value;
    }

    void PlayerController::Jump()
    {
        m_jumpPressed = true;
        if (m_pawn) {
            m_pawn->Jump();
        }
        m_jumpPressed = false; // one‑shot
    }

    void PlayerController::Fire()
    {
        m_firePressed = true;
        if (m_pawn) {
            m_pawn->Attack();
        }
        m_firePressed = false;
    }

    void PlayerController::Interact()
    {
        // To be implemented
    }

    void PlayerController::ApplyMovement(float deltaTime)
    {
        if (!m_pawn) return;

        // Get forward/right vectors from pawn orientation (or camera)
        // For simplicity, use pawn's transform forward/right.
        // In a first-person game, you'd use camera orientation.
        Vector3 forward = m_pawn->GetForward();
        Vector3 right   = m_pawn->GetRight();

        // Combine movement inputs
        Vector3 move = forward * m_moveInput.x + right * m_moveInput.y;
        if (move.LengthSq() > 0.01f) {
            move.Normalize();
            float speed = m_pawn->GetMoveSpeed();
            m_pawn->AddMovement(move * speed);
        } else {
            // stop
            m_pawn->AddMovement(Vector3::Zero);
        }
    }

    void PlayerController::ApplyLook(float deltaTime)
    {
        if (!m_pawn) return;

        // Rotate pawn based on look input (yaw for horizontal, pitch for vertical)
        float yaw = m_lookInput.x * deltaTime * 100.0f; // sensitivity scaling
        float pitch = m_lookInput.y * deltaTime * 100.0f;

        if (m_camera) {
            // In a first-person view, you'd rotate the camera and maybe the pawn's yaw.
            // For now, just rotate pawn.
            m_pawn->AddYaw(yaw);
            m_pawn->AddPitch(pitch);
        } else {
            m_pawn->AddYaw(yaw);
            m_pawn->AddPitch(pitch);
        }
    }

} // namespace USE