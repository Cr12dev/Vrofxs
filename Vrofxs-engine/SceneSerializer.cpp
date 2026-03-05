#include "include/SceneSerializer.h"
#include "include/Objects.h"
#include <iostream>
#include <algorithm>
#include <windows.h>
#include <commdlg.h>

// Implementación del singleton
SceneSerializer& SceneSerializer::GetInstance() {
    static SceneSerializer instance;
    return instance;
}

// Serializar un vector3
std::string SceneSerializer::SerializeVector3(const glm::vec3& vec) {
    return std::to_string(vec.x) + "," + std::to_string(vec.y) + "," + std::to_string(vec.z);
}

// Deserializar un vector3
glm::vec3 SceneSerializer::DeserializeVector3(const std::string& data) {
    glm::vec3 vec(0.0f);
    std::stringstream ss(data);
    std::string token;
    
    if (std::getline(ss, token, ',')) vec.x = std::stof(token);
    if (std::getline(ss, token, ',')) vec.y = std::stof(token);
    if (std::getline(ss, token, ',')) vec.z = std::stof(token);
    
    return vec;
}

// Escapar caracteres especiales
std::string SceneSerializer::EscapeString(const std::string& str) {
    std::string escaped = str;
    size_t pos = 0;
    
    while ((pos = escaped.find('\\', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "\\\\");
        pos += 2;
    }
    
    pos = 0;
    while ((pos = escaped.find('\n', pos)) != std::string::npos) {
        escaped.replace(pos, 1, "\\n");
        pos += 2;
    }
    
    return escaped;
}

// Desescapar caracteres especiales
std::string SceneSerializer::UnescapeString(const std::string& str) {
    std::string unescaped = str;
    size_t pos = 0;
    
    while ((pos = unescaped.find("\\n", pos)) != std::string::npos) {
        unescaped.replace(pos, 2, "\n");
        pos += 1;
    }
    
    pos = 0;
    while ((pos = unescaped.find("\\\\", pos)) != std::string::npos) {
        unescaped.replace(pos, 2, "\\");
        pos += 1;
    }
    
    return unescaped;
}

// Serializar Transform
std::string SceneSerializer::SerializeTransform(const Transform& transform) {
    std::string result = "transform{";
    result += "pos=" + SerializeVector3(transform.position) + ";";
    result += "rot=" + SerializeVector3(transform.rotation) + ";";
    result += "scl=" + SerializeVector3(transform.scale) + ";";
    result += "}";
    return result;
}

// Deserializar Transform
Transform SceneSerializer::DeserializeTransform(const std::string& data) {
    Transform transform;
    
    size_t start = data.find("transform{");
    if (start == std::string::npos) return transform;
    
    start += 9; // longitud de "transform{"
    size_t end = data.find("}", start);
    if (end == std::string::npos) return transform;
    
    std::string content = data.substr(start, end - start);
    std::stringstream ss(content);
    std::string token;
    
    while (std::getline(ss, token, ';')) {
        size_t equalPos = token.find('=');
        if (equalPos == std::string::npos) continue;
        
        std::string key = token.substr(0, equalPos);
        std::string value = token.substr(equalPos + 1);
        
        if (key == "pos") {
            transform.position = DeserializeVector3(value);
        } else if (key == "rot") {
            transform.rotation = DeserializeVector3(value);
        } else if (key == "scl") {
            transform.scale = DeserializeVector3(value);
        }
    }
    
    return transform;
}

// Serializar objeto individual
std::string SceneSerializer::SerializeObject(const SceneObject* obj) {
    if (!obj) return "";
    
    std::string result = "object{";
    result += "name=" + EscapeString(obj->name) + ";";
    result += "type=" + std::to_string(static_cast<int>(obj->type)) + ";";
    result += "visible=" + std::to_string(obj->visible ? 1 : 0) + ";";
    result += SerializeTransform(obj->transform) + ";";
    
    // Serializar hijos
    if (!obj->children.empty()) {
        result += "children=[";
        for (size_t i = 0; i < obj->children.size(); ++i) {
            if (i > 0) result += "|";
            result += SerializeObject(obj->children[i].get());
        }
        result += "];";
    }
    
    result += "}";
    return result;
}

// Deserializar objeto individual
std::shared_ptr<SceneObject> SceneSerializer::DeserializeObject(const std::string& data) {
    size_t start = data.find("object{");
    if (start == std::string::npos) return nullptr;
    
    start += 7; // longitud de "object{"
    size_t end = data.find("}", start);
    if (end == std::string::npos) return nullptr;
    
    std::string content = data.substr(start, end - start);
    
    // Extraer propiedades básicas
    std::string name = "Object";
    ObjectType type = ObjectType::Empty;
    bool visible = true;
    Transform transform;
    std::string childrenData;
    
    // Parsear contenido
    std::stringstream ss(content);
    std::string token;
    
    while (std::getline(ss, token, ';')) {
        size_t equalPos = token.find('=');
        if (equalPos == std::string::npos) continue;
        
        std::string key = token.substr(0, equalPos);
        std::string value = token.substr(equalPos + 1);
        
        if (key == "name") {
            name = UnescapeString(value);
        } else if (key == "type") {
            type = static_cast<ObjectType>(std::stoi(value));
        } else if (key == "visible") {
            visible = (std::stoi(value) != 0);
        } else if (key.find("transform{") == 0) {
            transform = DeserializeTransform(key + ";");
        } else if (key == "children") {
            childrenData = value;
        }
    }
    
    // Crear objeto
    auto obj = SceneManager::GetInstance().CreateObject(name, type);
    obj->transform = transform;
    obj->visible = visible;
    
    // Procesar hijos
    if (!childrenData.empty()) {
        size_t startChildren = childrenData.find('[');
        size_t endChildren = childrenData.find(']');
        if (startChildren != std::string::npos && endChildren != std::string::npos) {
            std::string childrenContent = childrenData.substr(startChildren + 1, endChildren - startChildren - 1);
            
            std::stringstream childSS(childrenContent);
            std::string childToken;
            
            while (std::getline(childSS, childToken, '|')) {
                auto child = DeserializeObject(childToken);
                if (child) {
                    obj->AddChild(child);
                }
            }
        }
    }
    
    return obj;
}

// Guardar escena completa
bool SceneSerializer::SaveScene(const std::string& filePath) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo para guardar: " << filePath << std::endl;
        return false;
    }
    
    // Escribir cabecera
    file << "VRO_SCENE_FILE_v1.0\n";
    file << "objects_count=" << SceneManager::GetInstance().objects.size() << "\n";
    
    // Guardar cada objeto raíz
    for (const auto& obj : SceneManager::GetInstance().objects) {
        file << SerializeObject(obj.get()) << "\n";
    }
    
    file.close();
    lastFilePath = filePath;
    
    std::cout << "Escena guardada exitosamente en: " << filePath << std::endl;
    return true;
}

// Cargar escena desde archivo
bool SceneSerializer::LoadScene(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo para cargar: " << filePath << std::endl;
        return false;
    }
    
    // Verificar cabecera
    std::string header;
    std::getline(file, header);
    if (header != "VRO_SCENE_FILE_v1.0") {
        std::cerr << "Error: Formato de archivo no válido" << std::endl;
        file.close();
        return false;
    }
    
    // Limpiar escena actual
    SceneManager::GetInstance().ClearAll();
    
    // Leer línea de conteo de objetos
    std::string countLine;
    std::getline(file, countLine);
    size_t equalPos = countLine.find('=');
    if (equalPos == std::string::npos) {
        std::cerr << "Error: Formato de conteo inválido" << std::endl;
        file.close();
        return false;
    }
    
    // Cargar objetos
    std::string objectData;
    while (std::getline(file, objectData)) {
        if (!objectData.empty()) {
            auto obj = DeserializeObject(objectData);
            if (obj) {
                SceneManager::GetInstance().objects.push_back(obj);
            }
        }
    }
    
    file.close();
    lastFilePath = filePath;
    
    std::cout << "Escena cargada exitosamente desde: " << filePath << std::endl;
    return true;
}

// Función para mostrar diálogo de guardar archivo
std::string ShowSaveFileDialog() {
    OPENFILENAMEA ofn;
    char szFile[260] = "scene.vro";
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetActiveWindow();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "VRO Scene Files\0*.vro\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = "vro";
    
    if (GetSaveFileNameA(&ofn)) {
        return std::string(ofn.lpstrFile);
    }
    return "";
}

// Función para mostrar diálogo de abrir archivo
std::string ShowOpenFileDialog() {
    OPENFILENAMEA ofn;
    char szFile[260] = "";
    
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetActiveWindow();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "VRO Scene Files\0*.vro\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    if (GetOpenFileNameA(&ofn)) {
        return std::string(ofn.lpstrFile);
    }
    return "";
}

// Funciones globales para facilitar el uso
bool SaveSceneToFile() {
    std::string filePath = ShowSaveFileDialog();
    if (filePath.empty()) {
        return false; // Usuario canceló el diálogo
    }
    
    return SceneSerializer::GetInstance().SaveScene(filePath);
}

bool LoadSceneFromFile() {
    std::string filePath = ShowOpenFileDialog();
    if (filePath.empty()) {
        return false; // Usuario canceló el diálogo
    }
    
    return SceneSerializer::GetInstance().LoadScene(filePath);
}
