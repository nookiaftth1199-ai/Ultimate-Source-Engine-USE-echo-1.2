// ============================================================
// Ultimate Source Engine - Inventory System Implementation
//============================================================
// ============================================================

#include "stdafx.h"
#include "InventorySystem.h"
#include "Entity/Entity.h"
#include <algorithm>

namespace USE {

    // -----------------------------------------------------------------
    // Item
    // -----------------------------------------------------------------
    Item::Item(const std::string& name, int id, int maxStack)
        : m_name(name), m_id(id), m_maxStack(maxStack)
    {}

    // -----------------------------------------------------------------
    // Inventory
    // -----------------------------------------------------------------
    Inventory::Inventory(int slotCount)
        : m_slots(slotCount)
    {
    }

    Inventory::~Inventory()
    {
        // Items are owned externally (by item database), so not deleted here.
    }

    void Inventory::Clear()
    {
        for (auto& slot : m_slots) {
            slot.item = nullptr;
            slot.count = 0;
        }
    }

    int Inventory::AddItem(Item* item, int amount)
    {
        if (!item || amount <= 0) return 0;

        // First try to stack with existing slots
        int remaining = amount;
        for (auto& slot : m_slots) {
            if (slot.item == item && slot.count < item->GetMaxStack()) {
                int space = item->GetMaxStack() - slot.count;
                int add = std::min(space, remaining);
                slot.count += add;
                remaining -= add;
                if (remaining == 0) return amount;
            }
        }

        // Then try empty slots
        for (auto& slot : m_slots) {
            if (slot.IsEmpty()) {
                slot.item = item;
                int add = std::min(item->GetMaxStack(), remaining);
                slot.count = add;
                remaining -= add;
                if (remaining == 0) return amount;
            }
        }

        return amount - remaining; // return number actually added
    }

    int Inventory::RemoveItem(int itemId, int amount)
    {
        if (amount <= 0) return 0;
        int remaining = amount;

        // Remove from slots (reverse order)
        for (auto& slot : m_slots) {
            if (!slot.IsEmpty() && slot.item->GetID() == itemId) {
                int remove = std::min(slot.count, remaining);
                slot.count -= remove;
                remaining -= remove;
                if (slot.count == 0) {
                    slot.item = nullptr;
                }
                if (remaining == 0) break;
            }
        }
        return amount - remaining;
    }

    bool Inventory::HasItem(int itemId, int amount) const
    {
        int total = 0;
        for (const auto& slot : m_slots) {
            if (!slot.IsEmpty() && slot.item->GetID() == itemId) {
                total += slot.count;
                if (total >= amount) return true;
            }
        }
        return false;
    }

    int Inventory::GetItemCount(int itemId) const
    {
        int total = 0;
        for (const auto& slot : m_slots) {
            if (!slot.IsEmpty() && slot.item->GetID() == itemId) {
                total += slot.count;
            }
        }
        return total;
    }

    bool Inventory::Transfer(Inventory& from, Inventory& to, int itemId, int amount)
    {
        if (!from.HasItem(itemId, amount)) return false;
        // Remove from 'from' first, then add to 'to'.
        int removed = from.RemoveItem(itemId, amount);
        if (removed == 0) return false;
        // We need the item definition. We'll get it from the first found slot.
        Item* item = nullptr;
        for (auto& slot : from.m_slots) {
            if (!slot.IsEmpty() && slot.item->GetID() == itemId) {
                item = slot.item;
                break;
            }
        }
        if (!item) return false; // shouldn't happen
        int added = to.AddItem(item, removed);
        if (added < removed) {
            // rollback? Not implemented.
        }
        return added > 0;
    }

    // -----------------------------------------------------------------
    // InventoryComponent
    // -----------------------------------------------------------------
    InventoryComponent::InventoryComponent(int slotCount)
        : m_inventory(slotCount)
    {
    }

    InventoryComponent::~InventoryComponent()
    {
    }

} // namespace USE