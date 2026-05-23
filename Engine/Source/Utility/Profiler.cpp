// ============================================================
// Ultimate Source Engine - Profiler Implementation
// ============================================================

#include "stdafx.h"
#include "Profiler.h"
#include "Core/Logger.h"
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace USE {

	// -----------------------------------------------------------------
	// Singleton instance
	// -----------------------------------------------------------------
	Profiler& Profiler::Get() {
		static Profiler instance;
		return instance;
	}

	// -----------------------------------------------------------------
	// Constructor / Destructor
	// -----------------------------------------------------------------
	Profiler::Profiler()
		: m_enabled(true)
		, m_maxEntries(500)
		, m_currentDepth(0)
	{
	}

	Profiler::~Profiler()
	{
	}

	// -----------------------------------------------------------------
	// BeginFrame: reset per‑frame accumulators and record start time
	// -----------------------------------------------------------------
	void Profiler::BeginFrame()
	{
		if (!m_enabled) return;
		m_frameStart = std::chrono::high_resolution_clock::now();
		// Clear previous frame results (they will be filled during EndFrame)
		m_accumulated.clear();
		// Ensure stack is empty (should be, but sanity)
		while (!m_blockStack.empty()) m_blockStack.pop();
		m_currentDepth = 0;
	}

	// -----------------------------------------------------------------
	// EndFrame: compute total frame time and prepare results
	// -----------------------------------------------------------------
	void Profiler::EndFrame()
	{
		if (!m_enabled) return;

		auto frameEnd = std::chrono::high_resolution_clock::now();
		double frameTime = std::chrono::duration<double, std::micro>(frameEnd - m_frameStart).count();

		// Add a special entry for the whole frame
		AddSample("TotalFrame", frameTime, frameTime, 0);

		// Convert accumulated map to vector for sorting
		m_frameResults.clear();
		m_frameResults.reserve(m_accumulated.size());
		for (auto& pair : m_accumulated) {
			m_frameResults.push_back(pair.second);
		}

		// Limit number of entries if necessary
		if (m_frameResults.size() > m_maxEntries) {
			// Keep only the top `m_maxEntries` by total time (partial sort)
			std::nth_element(m_frameResults.begin(),
				m_frameResults.begin() + m_maxEntries,
				m_frameResults.end(),
				[](const ProfileEntry& a, const ProfileEntry& b) {
				return a.timeMicroseconds > b.timeMicroseconds;
			});
			m_frameResults.resize(m_maxEntries);
		}

		// Optionally, we could sort by name or time here, but DumpResults will handle.
	}

	// -----------------------------------------------------------------
	// BeginBlock: push a new block onto the stack
	// -----------------------------------------------------------------
	void Profiler::BeginBlock(const std::string& name)
	{
		if (!m_enabled) return;

		Block block;
		block.name = name;
		block.start = std::chrono::high_resolution_clock::now();
		block.childrenTime = 0.0;
		block.depth = m_currentDepth;
		m_blockStack.push(block);
		m_currentDepth++;
	}

	// -----------------------------------------------------------------
	// EndBlock: pop the block and record its time
	// -----------------------------------------------------------------
	void Profiler::EndBlock()
	{
		if (!m_enabled) return;
		if (m_blockStack.empty()) {
			USE_LOG_WARN("Profiler::EndBlock called without matching BeginBlock");
			return;
		}

		Block block = m_blockStack.top();
		m_blockStack.pop();
		m_currentDepth--;

		auto end = std::chrono::high_resolution_clock::now();
		double elapsed = std::chrono::duration<double, std::micro>(end - block.start).count();

		// Subtract children time to get self time
		double selfTime = elapsed - block.childrenTime;

		AddSample(block.name, elapsed, selfTime, block.depth);
	}

	// -----------------------------------------------------------------
	// AddSample: update or create accumulated entry
	// -----------------------------------------------------------------
	void Profiler::AddSample(const std::string& name, double elapsedMicros, double selfMicros, int depth)
	{
		auto it = m_accumulated.find(name);
		if (it == m_accumulated.end()) {
			ProfileEntry entry;
			entry.name = name;
			entry.timeMicroseconds = elapsedMicros;
			entry.selfMicroseconds = selfMicros;
			entry.callCount = 1;
			entry.depth = depth;
			m_accumulated[name] = entry;
		}
		else {
			it->second.timeMicroseconds += elapsedMicros;
			it->second.selfMicroseconds += selfMicros;
			it->second.callCount++;
			// Depth is not updated after first entry; we keep the first depth (or update if needed)
		}

		// If we have a parent block (i.e., there is a block below this one on the stack),
		// add this block's elapsed time to its childrenTime.
		if (!m_blockStack.empty()) {
			// The top of the stack is the parent of the block we just finished.
			// But careful: when we call AddSample, the block has already been popped,
			// so the stack now contains the parent. However, the parent block was created
			// before the child and was still on the stack. When we popped the child,
			// we have the parent as the new top. So we can add to its childrenTime.
			// But we need to have a reference to the parent block. Since m_blockStack.top()
			// is the parent (because child was popped), we can add elapsedMicros to its childrenTime.
			// However, the parent block's childrenTime should be increased by the child's *total* time,
			// not self time. So we add elapsedMicros.
			// This requires that m_blockStack is a stack of references? Actually m_blockStack holds copies.
			// We need to modify the top element. We can do:
			Block& parent = const_cast<Block&>(m_blockStack.top());
			parent.childrenTime += elapsedMicros;
		}
	}

	// -----------------------------------------------------------------
	// Reset: clear all accumulated data
	// -----------------------------------------------------------------
	void Profiler::Reset()
	{
		m_accumulated.clear();
		m_frameResults.clear();
		while (!m_blockStack.empty()) m_blockStack.pop();
		m_currentDepth = 0;
	}

	// -----------------------------------------------------------------
	// DumpResults: output profiling data to the log
	// -----------------------------------------------------------------
	void Profiler::DumpResults(bool sortByTime)
	{
		if (!m_enabled) return;

		if (m_frameResults.empty()) {
			USE_LOG_INFO("Profiler: No data to dump.");
			return;
		}

		// Sort if requested
		if (sortByTime) {
			std::sort(m_frameResults.begin(), m_frameResults.end(),
				[](const ProfileEntry& a, const ProfileEntry& b) {
				return a.timeMicroseconds > b.timeMicroseconds;
			});
		}
		else {
			std::sort(m_frameResults.begin(), m_frameResults.end(),
				[](const ProfileEntry& a, const ProfileEntry& b) {
				return a.name < b.name;
			});
		}

		USE_LOG_INFO("========== Profiler Results ==========");
		USE_LOG_INFO("%-30s %12s %12s %8s", "Name", "Total (ms)", "Self (ms)", "Calls");
		USE_LOG_INFO("----------------------------------------------");

		for (const auto& entry : m_frameResults) {
			double totalMs = entry.timeMicroseconds / 1000.0;
			double selfMs = entry.selfMicroseconds / 1000.0;
			USE_LOG_INFO("%-30s %12.3f %12.3f %8d",
				entry.name.c_str(), totalMs, selfMs, entry.callCount);
		}
		USE_LOG_INFO("======================================");
	}

} // namespace USE