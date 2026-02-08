# 👨‍🍳 KitchenOS | Professional Recipe Management

KitchenOS is a high-performance, lightweight recipe management system built for professional kitchens. It emphasizes speed, readability for chefs, and a zero-dependency architecture.

## Getting Started (GitHub Codespaces)

This project is fully optimized and **tested for GitHub Codespaces**. To run the system:

1. **Open the Project** in a new GitHub Codespace.
2. **Execute the Build Script** in the Codespace terminal:
   ```bash
   chmod +x run.sh && ./run.sh

3. **Lauch the UI** Open the UI:
When the terminal displays ✅ **KitchenOS Backend is live**, click the **"Open in Browser"** pop-up.

If you miss the pop-up, go to the **Ports** tab in the bottom panel and click the link for port **8080**.

---

## Requirements Fulfillment
* **List/Instructions/Yield:** Fully implemented with a high-contrast kitchen UI.
* **Search:** Case-insensitive substring matching for fast lookup.
* **Multi-tenancy:** Uses `X-Restaurant-ID` headers to isolate data between competing establishments.
* **Scale:** Engineered to handle 10k requests/day and 1,000 recipes using thread-safe C++ memory management.

---

## Project Structure
* **backend/main.cpp:** The REST API server. Handles HTTP routing, JSON serialization, and connects the web layer to the business logic.
* **backend/include/RecipeManager.hpp:** The "Brain" of the app. Contains the RecipeService which handles thread-safe data storage and search algorithms.
* **backend/tests.cpp:** The automated test suite to ensure data integrity and API logic consistency.
* **backend/include/httplib.h**: A lightweight, single-header C++ HTTP library used to handle networking without requiring complex system installations (downloaded from https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h).
* **frontend/index.html:** The user interface. A single-page application (SPA) that communicates with the backend via fetch calls.
* **run.sh:** The build and execution script. A "one-click" solution to compile, test, and launch the entire system.

