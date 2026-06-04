// ============================================================
// Ultimate Source Engine - Player Controller
//============================================================
//
// Represents a player that can possess a pawn and handle input.
// Translates raw input into movement and actions for the possessed pawn.
// ============================================================

#pragma once

#include "stdafx.h"
#include "Math/Vector2.h"
#include <string>

namespace USE {

    // Forward declarations
    class Pawn;
    class CameraComponent;
    class InputManager;

    class PlayerController {
    public:
        PlayerController();
        virtual ~PlayerController();

        // Possess/unpossess a pawn
        void Possess(Pawn* pawn);
        void Unpossess();
        Pawn* GetPawn() const { return m_pawn; }

        // Camera management
        void SetCamera(CameraComponent* camera) { m_camera = camera; }
        CameraComponent* GetCamera() const { return m_camera; }

        // Initialize input bindings (called by GameMode)
        virtual void SetupInputBindings(InputManager* inputManager);

        // Update – called every frame (processes input and updates pawn)
        virtual void Update(float deltaTime);

        // Input handlers (override in derived classes)
        virtual void MoveForward(float value);
        virtual void MoveRight(float value);
        virtual void MoveUp(float value);   // for flying/climbing
        virtual void LookHorizontal(float value);
        virtual void LookVertical(float value);
        virtual void Jump();
        virtual void Fire();
        virtual void Interact();

    protected:
        Pawn*            m_pawn;
        CameraComponent* m_camera;

        // Movement state
        Vector2 m_moveInput;          // forward/right (x = forward, y = right)
        Vector2 m_lookInput;          // delta yaw/pitch (mouse or gamepad)
        bool    m_jumpPressed;
        bool    m_firePressed;

        // Helper to apply movement to pawn
        void ApplyMovement(float deltaTime);
        void ApplyLook(float deltaTime);
    };

} // namespace USE