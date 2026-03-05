#pragma once

#include "Main.h"
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Forward declarations
class Shader;

// Tipos de objetos soportados
enum class ObjectType {
    None,
    Cube,
    Sphere,
    Plane,
    Light,
    Camera,
    Empty
};

// Componente base Transform
struct Transform {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f); // Euler angles en grados
    glm::vec3 scale = glm::vec3(1.0f);
    
    glm::mat4 GetMatrix() const;
    void SetPosition(const glm::vec3& pos) { position = pos; }
    void SetRotation(const glm::vec3& rot) { rotation = rot; }
    void SetScale(const glm::vec3& scl) { scale = scl; }
};

// Clase base para todos los objetos de la escena
class SceneObject {
public:
    std::string name = "Object";
    ObjectType type = ObjectType::None;
    Transform transform;
    bool visible = true;
    bool selected = false;
    SceneObject* parent = nullptr;
    std::vector<std::shared_ptr<SceneObject>> children;
    
    SceneObject(const std::string& objName = "Object", ObjectType objType = ObjectType::Empty);
    virtual ~SceneObject() = default;
    
    // Jerarquía
    void SetParent(SceneObject* newParent);
    void AddChild(std::shared_ptr<SceneObject> child);
    void RemoveChild(SceneObject* child);
    std::shared_ptr<SceneObject> GetChild(int index) const;
    int GetChildCount() const { return (int)children.size(); }
    
    // Transformaciones globales
    glm::mat4 GetWorldMatrix() const;
    glm::vec3 GetWorldPosition() const;
    
    // Selección
    void SetSelected(bool sel) { selected = sel; }
    bool IsSelected() const { return selected; }
    
    // Rendering
    virtual void Render(Shader& shader, const glm::mat4& view, const glm::mat4& projection);
    virtual void Update(float deltaTime) {}
    
    // Utilidades
    virtual bool RaycastHit(const glm::vec3& rayOrigin, const glm::vec3& rayDir, float& distance) { return false; }
};

// Gestor de escena - singleton para manejar todos los objetos
class SceneManager {
public:
    static SceneManager& GetInstance();
    
    std::vector<std::shared_ptr<SceneObject>> objects;
    SceneObject* selectedObject = nullptr;
    
    // Crear objetos
    std::shared_ptr<SceneObject> CreateObject(const std::string& name, ObjectType type);
    std::shared_ptr<SceneObject> CreateCube(const std::string& name, const glm::vec3& position);
    std::shared_ptr<SceneObject> CreateEmpty(const std::string& name);
    
    // Eliminar objetos
    void DeleteObject(SceneObject* obj);
    void DeleteSelected();
    void ClearAll();
    
    // Selección
    void SelectObject(SceneObject* obj);
    void DeselectAll();
    SceneObject* GetSelected() const { return selectedObject; }
    SceneObject* RaycastSelect(const glm::vec3& rayOrigin, const glm::vec3& rayDir);
    
    // Buscar
    SceneObject* FindObject(const std::string& name);
    std::shared_ptr<SceneObject> FindObjectShared(const std::string& name);
    
    // Rendering
    void RenderAll(Shader& shader, const glm::mat4& view, const glm::mat4& projection);
    void UpdateAll(float deltaTime);
    
    // Utilidades
    int GetObjectCount() const { return (int)objects.size(); }
    std::vector<std::string> GetObjectNames() const;
    
private:
    SceneManager() = default;
    ~SceneManager() = default;
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;
};