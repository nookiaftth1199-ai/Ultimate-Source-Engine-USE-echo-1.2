// ============================================================
// Ultimate Source Engine - Behavior Tree
//============================================================
//
// Behavior tree implementation with composite, condition,
// and action nodes. Nodes return Status (Success, Failure, Running).
// ============================================================

#pragma once

#include "stdafx.h"
#include <vector>
#include <memory>
#include <functional>

namespace USE {

    // -----------------------------------------------------------------
    // Node status
    // -----------------------------------------------------------------
    enum class BTStatus {
        Invalid,
        Success,
        Failure,
        Running
    };

    // -----------------------------------------------------------------
    // Base node class
    // -----------------------------------------------------------------
    class BTNode {
    public:
        BTNode();
        virtual ~BTNode() = default;

        virtual BTStatus Tick(float deltaTime) = 0;
        virtual void Reset();

        BTStatus GetStatus() const { return m_status; }
        void SetStatus(BTStatus status) { m_status = status; }

    protected:
        BTStatus m_status;
    };

    // -----------------------------------------------------------------
    // Composite node (has children)
    // -----------------------------------------------------------------
    class BTComposite : public BTNode {
    public:
        void AddChild(std::unique_ptr<BTNode> child);
        void ClearChildren();

    protected:
        std::vector<std::unique_ptr<BTNode>> m_children;
    };

    // -----------------------------------------------------------------
    // Sequence node: runs children in order until one fails.
    // -----------------------------------------------------------------
    class BTSequence : public BTComposite {
    public:
        BTStatus Tick(float deltaTime) override;
        void Reset() override;

    private:
        size_t m_currentChild = 0;
    };

    // -----------------------------------------------------------------
    // Selector node: runs children in order until one succeeds.
    // -----------------------------------------------------------------
    class BTSelector : public BTComposite {
    public:
        BTStatus Tick(float deltaTime) override;
        void Reset() override;

    private:
        size_t m_currentChild = 0;
    };

    // -----------------------------------------------------------------
    // Parallel node: runs all children, succeeds if N succeed.
    // -----------------------------------------------------------------
    class BTParallel : public BTComposite {
    public:
        enum class Policy {
            RequireOne,      // success if at least one child succeeds
            RequireAll       // success if all children succeed
        };

        BTParallel(Policy successPolicy = Policy::RequireAll);

        BTStatus Tick(float deltaTime) override;
        void Reset() override;

    private:
        Policy m_successPolicy;
        std::vector<BTStatus> m_childStatus;
    };

    // -----------------------------------------------------------------
    // Condition node: checks a condition, returns Success or Failure.
    // -----------------------------------------------------------------
    using ConditionFunc = std::function<bool()>;

    class BTCondition : public BTNode {
    public:
        explicit BTCondition(ConditionFunc func);

        BTStatus Tick(float deltaTime) override;

    private:
        ConditionFunc m_func;
    };

    // -----------------------------------------------------------------
    // Action node: performs an action, returns Status.
    // -----------------------------------------------------------------
    using ActionFunc = std::function<BTStatus(float)>;

    class BTAction : public BTNode {
    public:
        explicit BTAction(ActionFunc func);

        BTStatus Tick(float deltaTime) override;

    private:
        ActionFunc m_func;
    };

    // -----------------------------------------------------------------
    // Decorator node: modifies the behavior of a single child.
    // -----------------------------------------------------------------
    class BTDecorator : public BTNode {
    public:
        explicit BTDecorator(std::unique_ptr<BTNode> child);

    protected:
        std::unique_ptr<BTNode> m_child;
    };

    // -----------------------------------------------------------------
    // Repeat decorator: repeats child a certain number of times.
    // -----------------------------------------------------------------
    class BTRepeat : public BTDecorator {
    public:
        BTRepeat(std::unique_ptr<BTNode> child, int repeatCount = -1);

        BTStatus Tick(float deltaTime) override;
        void Reset() override;

    private:
        int m_repeatCount;
        int m_currentCount;
    };

    // -----------------------------------------------------------------
    // Invert decorator: inverts the result of the child.
    // -----------------------------------------------------------------
    class BTInvert : public BTDecorator {
    public:
        explicit BTInvert(std::unique_ptr<BTNode> child);

        BTStatus Tick(float deltaTime) override;
    };

    // -----------------------------------------------------------------
    // Behavior tree (owns root node)
    // -----------------------------------------------------------------
    class BehaviorTree {
    public:
        BehaviorTree();
        explicit BehaviorTree(std::unique_ptr<BTNode> root);

        void SetRoot(std::unique_ptr<BTNode> root);
        BTStatus Tick(float deltaTime);
        void Reset();

    private:
        std::unique_ptr<BTNode> m_root;
    };

} // namespace USE