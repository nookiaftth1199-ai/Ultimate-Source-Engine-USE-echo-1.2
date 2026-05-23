#pragma once

// Ultimate Source Engine - Master Header
// Include this to use the entire engine

// Engine version
#include "Version.h"

// Core systems
#include "Core/Application.h"
#include "Core/FileSystem.h"
#include "Core/MemoryManager.h"
#include "Core/Timer.h"

// Math library
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix4.h"
#include "Math/Quaternion.h"

// Renderer
#include "Renderer/RenderSystem.h"
#include "Renderer/OpenGL/GLDevice.h"
#include "Renderer/Texture.h"
#include "Renderer/Shader.h"
#include "Renderer/Mesh.h"
#include "Renderer/Camera.h"
#include "Renderer/Light.h"

// Resources
#include "Resources/ResourceManager.h"
#include "Resources/FBXLoader.h"
#include "Resources/TextureLoader.h"
#include "Resources/MaterialLoader.h"

// Game framework
#include "GameFramework/GameMode.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

// AI
#include "AI/AIController.h"
#include "AI/BehaviorTree.h"
#include "AI/NavigationSystem.h"

// Physics
#include "Physics/PhysicsSystem.h"
#include "Physics/RigidBody.h"
#include "Physics/CharacterController.h"
#include "Physics/CollisionShape.h"

// Animation
#include "Animation/AnimationComponent.h"
#include "Animation/SkeletalMesh.h"

// UI
#include "UI/UISystem.h"
#include "UI/ConsoleSystem.h"

// Audio
#include "Audio/AudioSystem.h"
#include "Audio/SoundSource.h"

// Network
#include "Network/NetworkManager.h"

// Utility
#include "Utility/Logger.h"
#include "Utility/Profiler.h"
#include "Utility/Random.h"

// Global engine instance
namespace USE {
    extern Application* g_pUSE_Application;
}