// ============================================================
// Ultimate Source Engine - Behavior Tree Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "BehaviorTree.h"

namespace USE {

    // -----------------------------------------------------------------
    // BTNode
    // -----------------------------------------------------------------
    BTNode::BTNode() : m_status(BTStatus::Invalid) {}
    void BTNode::Reset() { m_status = BTStatus::Invalid; }

    // -----------------------------------------------------------------
    // BTComposite
    // -----------------------------------------------------------------
    void BTComposite::AddChild(std::unique_ptr<BTNode> child) {
        m_children.push_back(std::move(child));
    }
    void BTComposite::ClearChildren() { m_children.clear(); }

    // -----------------------------------------------------------------
    // BTSequence
    // -----------------------------------------------------------------
    BTStatus BTSequence::Tick(float deltaTime) {
        if (m_children.empty()) return BTStatus::Success;

        // Continue from current child
        for (size_t i = m_currentChild; i < m_children.size(); ++i) {
            BTStatus status = m_children[i]->Tick(deltaTime);
            if (status == BTStatus::Running) {
                m_currentChild = i;
                return BTStatus::Running;
            }
            if (status == BTStatus::Failure) {
                Reset();
                return BTStatus::Failure;
            }
            // If success, move to next child
        }
        // All children succeeded
        Reset();
        return BTStatus::Success;
    }
    void BTSequence::Reset() {
        BTNode::Reset();
        m_currentChild = 0;
        for (auto& child : m_children) child->Reset();
    }

    // -----------------------------------------------------------------
    // BTSelector
    // -----------------------------------------------------------------
    BTStatus BTSelector::Tick(float deltaTime) {
        if (m_children.empty()) return BTStatus::Failure;

        for (size_t i = m_currentChild; i < m_children.size(); ++i) {
            BTStatus status = m_children[i]->Tick(deltaTime);
            if (status == BTStatus::Running) {
                m_currentChild = i;
                return BTStatus::Running;
            }
            if (status == BTStatus::Success) {
                Reset();
                return BTStatus::Success;
            }
            // If failure, try next child
        }
        // All children failed
        Reset();
        return BTStatus::Failure;
    }
    void BTSelector::Reset() {
        BTNode::Reset();
        m_currentChild = 0;
        for (auto& child : m_children) child->Reset();
    }

    // -----------------------------------------------------------------
    // BTParallel
    // -----------------------------------------------------------------
    BTParallel::BTParallel(Policy successPolicy)
        : m_successPolicy(successPolicy) {}

    BTStatus BTParallel::Tick(float deltaTime) {
        if (m_children.empty()) return BTStatus::Success;

        // Ensure child status array size matches
        if (m_childStatus.size() != m_children.size()) {
            m_childStatus.resize(m_children.size(), BTStatus::Invalid);
        }

        int successCount = 0;
        int failureCount = 0;

        for (size_t i = 0; i < m_children.size(); ++i) {
            if (m_childStatus[i] == BTStatus::Running || m_childStatus[i] == BTStatus::Invalid) {
                BTStatus status = m_children[i]->Tick(deltaTime);
                m_childStatus[i] = status;
            }

            if (m_childStatus[i] == BTStatus::Success) successCount++;
            else if (m_childStatus[i] == BTStatus::Failure) failureCount++;
        }

        if (m_successPolicy == Policy::RequireOne && successCount >= 1) {
            Reset();
            return BTStatus::Success;
        }
        if (m_successPolicy == Policy::RequireAll && successCount == (int)m_children.size()) {
            Reset();
            return BTStatus::Success;
        }

        // If any child is still running, we are running
        for (auto s : m_childStatus) {
            if (s == BTStatus::Running) return BTStatus::Running;
        }

        // All children terminated without meeting success condition
        Reset();
        return BTStatus::Failure;
    }
    void BTParallel::Reset() {
        BTNode::Reset();
        m_childStatus.clear();
        for (auto& child : m_children) child->Reset();
    }

    // -----------------------------------------------------------------
    // BTCondition
    // -----------------------------------------------------------------
    BTCondition::BTCondition(ConditionFunc func) : m_func(func) {}
    BTStatus BTCondition::Tick(float deltaTime) {
        (void)deltaTime;
        bool result = m_func ? m_func() : false;
        m_status = result ? BTStatus::Success : BTStatus::Failure;
        return m_status;
    }

    // -----------------------------------------------------------------
    // BTAction
    // -----------------------------------------------------------------
    BTAction::BTAction(ActionFunc func) : m_func(func) {}
    BTStatus BTAction::Tick(float deltaTime) {
        if (m_func) {
            m_status = m_func(deltaTime);
        } else {
            m_status = BTStatus::Failure;
        }
        return m_status;
    }

    // -----------------------------------------------------------------
    // BTDecorator
    // -----------------------------------------------------------------
    BTDecorator::BTDecorator(std::unique_ptr<BTNode> child)
        : m_child(std::move(child)) {}

    // -----------------------------------------------------------------
    // BTRepeat
    // -----------------------------------------------------------------
    BTRepeat::BTRepeat(std::unique_ptr<BTNode> child, int repeatCount)
        : BTDecorator(std::move(child)), m_repeatCount(repeatCount), m_currentCount(0) {}

    BTStatus BTRepeat::Tick(float deltaTime) {
        if (!m_child) return BTStatus::Failure;

        BTStatus status = m_child->Tick(deltaTime);
        if (status == BTStatus::Running) {
            return BTStatus::Running;
        }

        // Child finished
        if (m_repeatCount == -1 || m_currentCount < m_repeatCount - 1) {
            m_currentCount++;
            m_child->Reset();
            return BTStatus::Running; // continue repeating
        }

        // Reached desired count
        Reset();
        return status; // return last status
    }
    void BTRepeat::Reset() {
        BTNode::Reset();
        m_currentCount = 0;
        if (m_child) m_child->Reset();
    }

    // -----------------------------------------------------------------
    // BTInvert
    // -----------------------------------------------------------------
    BTInvert::BTInvert(std::unique_ptr<BTNode> child) : BTDecorator(std::move(child)) {}

    BTStatus BTInvert::Tick(float deltaTime) {
        if (!m_child) return BTStatus::Failure;
        BTStatus status = m_child->Tick(deltaTime);
        if (status == BTStatus::Success) {
            m_status = BTStatus::Failure;
        } else if (status == BTStatus::Failure) {
            m_status = BTStatus::Success;
        } else {
            m_status = status; // running or invalid
        }
        return m_status;
    }

    // -----------------------------------------------------------------
    // BehaviorTree
    // -----------------------------------------------------------------
    BehaviorTree::BehaviorTree() : m_root(nullptr) {}
    BehaviorTree::BehaviorTree(std::unique_ptr<BTNode> root) : m_root(std::move(root)) {}

    void BehaviorTree::SetRoot(std::unique_ptr<BTNode> root) {
        m_root = std::move(root);
    }

    BTStatus BehaviorTree::Tick(float deltaTime) {
        if (!m_root) return BTStatus::Failure;
        return m_root->Tick(deltaTime);
    }

    void BehaviorTree::Reset() {
        if (m_root) m_root->Reset();
    }

} // namespace USE