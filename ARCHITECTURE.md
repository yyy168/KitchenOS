# KitchenOS Architecture Review

## Design Philosophy
The system follows a **Minimalist High-Performance** approach. By choosing **C++17** for the backend, we demonstrate high-performance capabilities and memory efficiency. By avoiding heavy frameworks, the entire application remains under 1MB, boots in milliseconds, and has a near-zero memory footprint.

## Data Strategy & Concurrency
Given the requirements of 1,000 recipes and 10,000 requests/day, a traditional SQL database was bypassed in favor of a thread-safe in-memory store.
- **Zero-Dependency Deployment:** A primary goal was to ensure the project "turns over" immediately in a GitHub Codespace without requiring the interviewer to install, configure, or manage external database servers (like PostgreSQL) or local development libraries (like libsqlite3-dev).
- **In-Memory Storage:** Used a `std::vector` of structs for sub-millisecond data access.
- **Concurrency:** Implemented `std::shared_mutex` (Readers-Writer lock) to allow simultaneous search operations while maintaining thread safety during recipe edits.

## 💾 Data Persistence & Production Readiness
- **Current State:** The system uses a volatile in-memory store. Data is reset on server restart. 
- **Reasoning:** This was chosen to fulfill the "Zero-Dependency" requirement, ensuring the project runs instantly in any environment without database configuration.
- **Production Roadmap:** In a real-world scenario, this `RecipeService` would be updated to:
    1. **PostgreSQL/MySQL:** For a robust, scalable multi-user production environment.
- **Architectural Flexibility:** Because the data logic is encapsulated within the `RecipeService` class, the underlying storage can be swapped for a database driver without modifying the API routing or Frontend logic.

## Multi-tenancy & Security
- **Isolation:** Implemented using an `X-Restaurant-ID` header. Data is partitioned by this ID so Restaurant A can never view Restaurant B's proprietary recipes.
- **Scalability:** The architecture is "Stateless-ready." The `RecipeService` is abstracted so it could be swapped for a Redis or SQL provider in the future without changing the API layer.

## Search & Edge Cases
The search logic uses a **Normalized Substring Match** to handle real-world kitchen conditions:
- **Case-Insensitivity:** Both the search query and titles are transformed to lowercase using `std::transform`. This prevents "human-error" bugs (e.g., "soup" vs "SOUP").
- **Robustness:** Includes server-side error catching to prevent crashes on malformed JSON and graceful handling of empty search results.

## Frontend Considerations
- **Environment:** Kitchens are high-pressure environments where chefs may use older tablets or touchscreens.
- **Solution:** A **Vanilla JS/CSS** approach was chosen to keep the system "zero-dependency" and extremely fast to load on any device without requiring a modern build pipeline.

## Testing Strategy
To ensure reliability, a custom test suite was implemented in `backend/tests.cpp`:
- **Functional Testing:** Validates case-insensitive substring search.
- **Security Testing:** Verifies multi-tenant data isolation.
- **Lifecycle Testing:** Confirms deletion logic and data integrity for yield/instructions.
- **CI Simulation:** The `run.sh` script automatically runs these tests; the server will refuse to start if any core logic fails.

