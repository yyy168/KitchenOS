#include "RecipeManager.hpp" // Ensure path is correct for your project structure
#include <cassert>
#include <iostream>
#include <vector>

void testSearchCapabilities() {
    RecipeService service;
    // Format: {id, restaurantId, title, ingredients, instructions, yield}
    service.addRecipe({0, 1, "Spaghetti Carbonara", "Eggs, Cheese, Guanciale", "Cook it.", "2p"});
    
    // 1. Case Insensitivity
    assert(service.search(1, "SPAGHETTI").size() == 1);
    
    // 2. Partial Match
    assert(service.search(1, "carbon").size() == 1);
    
    // 3. Ingredient Search (The new feature!)
    assert(service.search(1, "guanciale").size() == 1);
    assert(service.search(1, "Eggs").size() == 1);
}

void testMultiTenancy() {
    RecipeService service;
    service.addRecipe({0, 1, "RestA_Dish", "Ingreds", "Prep", "1p"});
    service.addRecipe({0, 2, "RestB_Dish", "Ingreds", "Prep", "1p"});
    
    // Verify Restaurant 1 cannot see Restaurant 2's data
    assert(service.search(1, "").size() == 1);
    assert(service.search(1, "RestB").size() == 0);
}

void testRecipeDeletion() {
    RecipeService service;
    // Capture the ID directly from the return value (Improved API)
    int assigned_id = service.addRecipe({0, 1, "Temp Dish", "Ingreds", "Delete me", "1p"});
    
    assert(assigned_id != -1);
    
    // Delete and Verify
    bool deleted = service.deleteRecipe(1, assigned_id);
    assert(deleted == true);
    assert(service.search(1, "").size() == 0);
    
    // Try to delete again (should fail)
    bool deleted_again = service.deleteRecipe(1, assigned_id);
    assert(deleted_again == false);
}

void testDataIntegrity() {
    RecipeService service;
    service.addRecipe({0, 1, "Yield Test", "Salt, Water", "Instructions", "4 Servings"});
    auto results = service.search(1, "Yield Test");
    
    // Verify all fields are preserved correctly
    assert(results[0].title == "Yield Test");
    assert(results[0].ingredients == "Salt, Water");
    assert(results[0].yield == "4 Servings");
    assert(results[0].instructions == "Instructions");
}

int main() {
    try {
        testSearchCapabilities();
        testMultiTenancy();
        testRecipeDeletion();
        testDataIntegrity();
        std::cout << "--- All Tests Passed Successfully! ---" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "❌ Tests failed with error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Tests failed with an unknown exception." << std::endl;
        return 1;
    }
    return 0;
}