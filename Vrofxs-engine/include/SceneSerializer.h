#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <memory>
#include <glm/glm.hpp>

// Forward declarations
class SceneObject;
class SceneManager;
struct Transform;

// Clase para serializar y deserializar escenas
class SceneSerializer {
public:
    static SceneSerializer& GetInstance();
    
    // Guardar escena completa a archivo .vro
    bool SaveScene(const std::string& filePath);
    
    // Cargar escena desde archivo .vro
    bool LoadScene(const std::string& filePath);
    
    // Serializar objeto individual
    std::string SerializeObject(const SceneObject* obj);
    
    // Deserializar objeto individual
    std::shared_ptr<SceneObject> DeserializeObject(const std::string& data);
    
    // Obtener última ruta de archivo
    std::string GetLastFilePath() const { return lastFilePath; }
    
private:
    SceneSerializer() = default;
    ~SceneSerializer() = default;
    SceneSerializer(const SceneSerializer&) = delete;
    SceneSerializer& operator=(const SceneSerializer&) = delete;
    
    std::string lastFilePath;
    
    // Funciones auxiliares de serialización
    std::string SerializeTransform(const Transform& transform);
    Transform DeserializeTransform(const std::string& data);
    
    std::string SerializeVector3(const glm::vec3& vec);
    glm::vec3 DeserializeVector3(const std::string& data);
    
    std::string EscapeString(const std::string& str);
    std::string UnescapeString(const std::string& str);
};

// Funciones globales para facilitar el uso
bool SaveSceneToFile();
bool LoadSceneFromFile();
