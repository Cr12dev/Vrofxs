#include "include/Objects.h"
#include "include/Shader.h"
#include <algorithm>
#include <iostream>

// ============================================================================
// TRANSFORM
// ============================================================================

glm::mat4 Transform::GetMatrix() const {
    glm::mat4 mat = glm::mat4(1.0f);
    mat = glm::translate(mat, position);
    mat = glm::rotate(mat, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    mat = glm::rotate(mat, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    mat = glm::rotate(mat, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    mat = glm::scale(mat, scale);
    return mat;
}

// ============================================================================
// SCENE OBJECT
// ============================================================================

SceneObject::SceneObject(const std::string& objName, ObjectType objType) 
    : name(objName), type(objType), parent(nullptr) {
}

void SceneObject::SetParent(SceneObject* newParent) {
    if (parent == newParent) return;
    
    // Remover de parent anterior
    if (parent) {
        auto it = std::find_if(parent->children.begin(), parent->children.end(),
            [this](const std::shared_ptr<SceneObject>& child) { return child.get() == this; });
        if (it != parent->children.end()) {
            parent->children.erase(it);
        }
    }
    
    parent = newParent;
}

void SceneObject::AddChild(std::shared_ptr<SceneObject> child) {
    if (!child || child.get() == this) return;
    
    // Remover de parent anterior
    if (child->parent) {
        child->SetParent(nullptr);
    }
    
    child->parent = this;
    children.push_back(child);
}

void SceneObject::RemoveChild(SceneObject* child) {
    auto it = std::find_if(children.begin(), children.end(),
        [child](const std::shared_ptr<SceneObject>& c) { return c.get() == child; });
    
    if (it != children.end()) {
        (*it)->parent = nullptr;
        children.erase(it);
    }
}

std::shared_ptr<SceneObject> SceneObject::GetChild(int index) const {
    if (index >= 0 && index < (int)children.size()) {
        return children[index];
    }
    return nullptr;
}

glm::mat4 SceneObject::GetWorldMatrix() const {
    glm::mat4 local = transform.GetMatrix();
    if (parent) {
        return parent->GetWorldMatrix() * local;
    }
    return local;
}

glm::vec3 SceneObject::GetWorldPosition() const {
    glm::vec4 pos = GetWorldMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    return glm::vec3(pos);
}

void SceneObject::Render(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    if (!visible) return;
    
    // Base implementation - subclasses override this
    shader.use();
    shader.setMat4("model", GetWorldMatrix());
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
}

// ============================================================================
// SCENE MANAGER (Singleton)
// ============================================================================

SceneManager& SceneManager::GetInstance() {
    static SceneManager instance;
    return instance;
}

std::shared_ptr<SceneObject> SceneManager::CreateObject(const std::string& name, ObjectType type) {
    auto obj = std::make_shared<SceneObject>(name, type);
    objects.push_back(obj);
    std::cout << "Created object: " << name << " (Type: " << (int)type << ")" << std::endl;
    return obj;
}

std::shared_ptr<SceneObject> SceneManager::CreateCube(const std::string& name, const glm::vec3& position) {
    auto obj = std::make_shared<SceneObject>(name, ObjectType::Cube);
    obj->transform.position = position;
    objects.push_back(obj);
    std::cout << "Created cube: " << name << " at (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
    return obj;
}

std::shared_ptr<SceneObject> SceneManager::CreateEmpty(const std::string& name) {
    auto obj = std::make_shared<SceneObject>(name, ObjectType::Empty);
    objects.push_back(obj);
    std::cout << "Created empty object: " << name << std::endl;
    return obj;
}

void SceneManager::DeleteObject(SceneObject* obj) {
    if (!obj) return;
    
    // Deseleccionar si es necesario
    if (selectedObject == obj) {
        selectedObject = nullptr;
    }
    
    // Remover de parent si tiene
    if (obj->parent) {
        obj->parent->RemoveChild(obj);
    }
    
    // Eliminar de la lista
    auto it = std::find_if(objects.begin(), objects.end(),
        [obj](const std::shared_ptr<SceneObject>& o) { return o.get() == obj; });
    
    if (it != objects.end()) {
        std::cout << "Deleted object: " << (*it)->name << std::endl;
        objects.erase(it);
    }
}

void SceneManager::DeleteSelected() {
    if (selectedObject) {
        DeleteObject(selectedObject);
        selectedObject = nullptr;
    }
}

void SceneManager::ClearAll() {
    objects.clear();
    selectedObject = nullptr;
    std::cout << "Scene cleared - all objects deleted" << std::endl;
}

void SceneManager::SelectObject(SceneObject* obj) {
    // Deseleccionar anterior
    DeselectAll();
    
    // Seleccionar nuevo
    selectedObject = obj;
    if (obj) {
        obj->selected = true;
        std::cout << "Selected object: " << obj->name << std::endl;
    }
}

void SceneManager::DeselectAll() {
    for (auto& obj : objects) {
        obj->selected = false;
    }
    selectedObject = nullptr;
}

SceneObject* SceneManager::RaycastSelect(const glm::vec3& rayOrigin, const glm::vec3& rayDir) {
    SceneObject* closest = nullptr;
    float closestDist = FLT_MAX;
    
    for (auto& obj : objects) {
        float distance;
        if (obj->RaycastHit(rayOrigin, rayDir, distance)) {
            if (distance < closestDist) {
                closestDist = distance;
                closest = obj.get();
            }
        }
    }
    
    if (closest) {
        SelectObject(closest);
    }
    
    return closest;
}

SceneObject* SceneManager::FindObject(const std::string& name) {
    for (auto& obj : objects) {
        if (obj->name == name) {
            return obj.get();
        }
    }
    return nullptr;
}

std::shared_ptr<SceneObject> SceneManager::FindObjectShared(const std::string& name) {
    for (auto& obj : objects) {
        if (obj->name == name) {
            return obj;
        }
    }
    return nullptr;
}

void SceneManager::RenderAll(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    for (auto& obj : objects) {
        obj->Render(shader, view, projection);
    }
}

void SceneManager::UpdateAll(float deltaTime) {
    for (auto& obj : objects) {
        obj->Update(deltaTime);
    }
}

std::vector<std::string> SceneManager::GetObjectNames() const {
    std::vector<std::string> names;
    for (const auto& obj : objects) {
        names.push_back(obj->name);
    }
    return names;
}
