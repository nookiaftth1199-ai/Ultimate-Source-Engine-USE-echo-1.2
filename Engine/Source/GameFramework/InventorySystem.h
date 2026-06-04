// ============================================================
// Ultimate Source Engine - Inventory System
//============================================================
//
// Provides item and inventory management for entities.
// Includes item definitions, inventory containers, and
// an optional system for synchronization.
// ============================================================

#pragma once

#include "stdafx.h"
#include <vector>
#include <string>

namespace USE {

    // Forward declarations
    class Entity;
    class Item;

    // -----------------------------------------------------------------
    // Item definition (base class)
    // -----------------------------------------------------------------
    class Item {
    public:
        Item(const std::string& name, int id, int maxStack = 1);
        virtual ~Item() = default;

        const std::string& GetName() const { return m_name; }
        int GetID() const { return m_id; }
        int GetMaxStack() const { return m_maxStack; }

    protected:
        std::string m_name;
        int         m_id;
        int         m_maxStack;
    };

    // -----------------------------------------------------------------
    // Inventory slot (contains an item and count)
    // -----------------------------------------------------------------
    struct InventorySlot {
        Item* item;
        int   count;

        InventorySlot() : item(nullptr), count(0) {}
        InventorySlot(Item* item, int count) : item(item), count(count) {}
        bool IsEmpty() const { return item == nullptr || count == 0; }
    };

    // -----------------------------------------------------------------
    // Inventory (container of slots)
    // -----------------------------------------------------------------
    class Inventory {
    public:
        explicit Inventory(int slotCount = 20);
        ~Inventory();

        // Clear all slots
        void Clear();

        // Add an item (returns number added)
        int AddItem(Item* item, int amount = 1);

        // Remove an item (returns number removed)
        int RemoveItem(int itemId, int amount = 1);

        // Check if contains at least 'amount' of an item
        bool HasItem(int itemId, int amount = 1) const;

        // Get total count of an item
        int GetItemCount(int itemId) const;

        // Get slot information
        const InventorySlot& GetSlot(int index) const { return m_slots[index]; }
        int GetSlotCount() const { return (int)m_slots.size(); }

        // Transfer items between inventories (returns true if successful)
        static bool Transfer(Inventory& from, Inventory& to, int itemId, int amount);

    private:
        std::vector<InventorySlot> m_slots;
    };

    // -----------------------------------------------------------------
    // InventoryComponent (attach to an entity)
    // -----------------------------------------------------------------
    class InventoryComponent : public Component {
    public:
        InventoryComponent(int slotCount = 20);
        virtual ~InventoryComponent();

        Inventory& GetInventory() { return m_inventory; }
        const Inventory& GetInventory() const { return m_inventory; }

        virtual const char* GetTypeName() const override { return "InventoryComponent"; }

    private:
        Inventory m_inventory;
    };

} // namespace USE