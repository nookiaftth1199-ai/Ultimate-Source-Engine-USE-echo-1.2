// ============================================================
// Ultimate Source Engine - Profiler
// ============================================================
//
// High‑performance profiling system for measuring execution times.
// Supports scoped blocks (RAII), hierarchical timings, and frame‑based aggregation.
// ============================================================

#pragma once

#include "stdafx.h"
#include <string>
#include <vector>
#include <chrono>
#include <stack>
#include <unordered_map>

namespace USE {

	// -----------------------------------------------------------------
	// Single profile entry (for a block)
	// -----------------------------------------------------------------
	struct ProfileEntry {
		std::string name;
		double      timeMicroseconds;   // total time for this block (including children)
		double      selfMicroseconds;   // exclusive time (excluding children)
		int         callCount;
		int         depth;               // nesting level (for hierarchical display)

		ProfileEntry() : timeMicroseconds(0.0), selfMicroseconds(0.0), callCount(0), depth(0) {}
	};

	// -----------------------------------------------------------------
	// Main Profiler class (singleton)
	// -----------------------------------------------------------------
	class Profiler {
	public:
		static Profiler& Get();

		// Start/end frame (call at beginning/end of main loop)
		void BeginFrame();
		void EndFrame();

		// Begin a named block (push onto stack)
		void BeginBlock(const std::string& name);

		// End the current block (pop and record)
		void EndBlock();

		// Enable/disable profiling (disabled = no overhead)
		void SetEnabled(bool enabled) { m_enabled = enabled; }
		bool IsEnabled() const { return m_enabled; }

		// Get results for the last frame (processed)
		const std::vector<ProfileEntry>& GetFrameResults() const { return m_frameResults; }

		// Reset accumulated results (optional)
		void Reset();

		// Output results to log (or console)
		void DumpResults(bool sortByTime = true);

		// Set maximum number of entries to store
		void SetMaxEntries(size_t max) { m_maxEntries = max; }

	private:
		Profiler();
		~Profiler();

		struct Block {
			std::string name;
			std::chrono::high_resolution_clock::time_point start;
			double      childrenTime;   // accumulated time of direct children (in microseconds)
			int         depth;
		};

		bool m_enabled;
		std::stack<Block> m_blockStack;
		std::unordered_map<std::string, ProfileEntry> m_accumulated;
		std::vector<ProfileEntry> m_frameResults;
		std::chrono::high_resolution_clock::time_point m_frameStart;
		size_t m_maxEntries;
		int   m_currentDepth;

		// Helper to update accumulated entry with a new sample
		void AddSample(const std::string& name, double elapsedMicros, double selfMicros, int depth);
	};

	// -----------------------------------------------------------------
	// RAII helper for scoped profiling
	// -----------------------------------------------------------------
	class ProfileBlock {
	public:
		ProfileBlock(const std::string& name) {
			Profiler::Get().BeginBlock(name);
		}
		~ProfileBlock() {
			Profiler::Get().EndBlock();
		}
	};

} // namespace USE

// Convenience macro
#define USE_PROFILE_SCOPE(name) USE::ProfileBlock _profile_block_##__LINE__(name)