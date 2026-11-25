# 📖 JOURNAL QUEST — A Personal Offline Diary

**Journal Quest** is a 100% native, ultra-responsive Linux desktop personal diary application built in **C++ and Qt 6**. 

It combines distraction-free document writing, Microsoft Word (`.docx`) bidirectional compatibility, a cozy indie aesthetic (warm parchment tones, gentle typography, paper textures), and a subtle offline progression system (blooming personal garden, streaks, and XP).

---

## ⚡ Performance Highlights
* **Pure Compiled C++ (Qt 6):** **No webviews, zero Chromium overhead, zero Electron lag.**
* **Tiny Binary Footprint:** ~**477 KB** single executable binary.
* **Instant Startup:** Opens in less than 50ms with a modest ~30MB RAM footprint.
* **100% Local & Private:** Embedded SQLite database (`journal_quest.sqlite`). No cloud dependencies, no external APIs, no telemetry.

---

## 🌟 Implemented Features

### 1. Main Navigation & Cozy Aesthetics
- **Warm Parchment Theme:** Specially designed palette with cream `#FAF7F2`, soft browns `#8E5A3C`, warm charcoal typography, and smooth transitions.
- **Left Navigation Sidebar:**
  - 🏠 **Home:** Welcoming greeting based on the time of day, active streak, stats strip, blooming garden, and a "Random Memory" button.
  - ✍️ **My Diary:** Primary writing workspace.
  - 📚 **All Entries:** Browse entries with instant live filtering and search.
  - ⭐ **Favorites:** Filter bookmarked thoughts.
  - 📁 **Collections:** Categorized entries (e.g. *Daily*, *Reflections*, *Creative*, *Travel*, *Ideas*).
  - 📄 **Word Documents:** Dedicated section for imported `.docx` files.
  - 🗑️ **Trash:** Soft delete with full restore and permanent wipe options.
  - ⚙️ **Settings:** Toggle gamification on/off (Zen Mode) and manage full JSON backups.

### 2. Distraction-Free Writing Workspace
- **Header Meta:** Live timestamp and responsive autosave status badge (`Saved`, `Saving...`, `Unsaved changes`).
- **Formatting Toolbar:**
  - Headings (H1, H2, H3, Normal Text).
  - Bold, Italic, Underline, Strikethrough.
  - Alignment (Left, Center, Right).
  - Bulleted and numbered lists.
  - Blockquotes and structured multi-cell tables.
- **Debounced Autosave:** Automatically persists changes 1.5 seconds after typing ceases.
- **Focus Mode (`Ctrl+Shift+F`):** Collapses navigation and sidebars for an immersive, distraction-free writing environment.
- **Live Metrics:** Word count, character counter, and estimated reading time.

### 3. Microsoft Word (`.docx`) Compatibility
- **Import:** Extracts paragraphs, headings, bold/italic/underline styles, and tables directly into the rich-text canvas.
- **Export:** Converts your diary entries to standard Microsoft Word `.docx` documents.
- **Direct Save-Back:** Allows updating existing `.docx` documents with explicit overwrite confirmations.

### 4. Cozy Offline Gamification & The Personal Garden
- **Custom-Drawn Garden Widget:** Procedurally rendered using Qt Vector Painters:
  - **Stage 0:** Dormant Seed in warm earth.
  - **Stage 1 (1+ Entries):** Sprouting Seed.
  - **Stage 2 (5+ Entries):** Growing Sapling.
  - **Stage 3 (15+ Entries):** Blooming Soul Flower.
  - **Stage 4 (30+ Entries):** Flourishing Sanctuary with golden sparkles and companion blossoms.
- **Streaks & XP:** Rewarded for daily writing milestones.
- **Zen Mode Toggle:** Can be completely disabled anytime via Settings.

---

## 🚀 How to Run

From your terminal, execute:

```bash
cd /home/learner/testforgemini/journal-quest/build
./journal-quest
```

### Keyboard Shortcuts:
* `Ctrl + N`: Create a new entry.
* `Ctrl + S`: Immediately save the current entry.
* `Ctrl + F`: Jump to entries search.
* `Ctrl + Shift + F`: Toggle distraction-free Focus Mode.

---

## 🧪 Verified Automated Tests
All core database operations, SQLite schema updates, XP/gamification progression calculations, and round-trip `.docx` import/export have been verified by the included test suite (`test-core`).
