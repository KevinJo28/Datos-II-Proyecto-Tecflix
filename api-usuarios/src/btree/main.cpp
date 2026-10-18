#include "BTree.hpp"

struct Usuario {
    int id;
    char username[50];
    char passwordHash[64];

    // Operadores requeridos por el B-Tree para comparar claves
    bool operator>(const Usuario& other) const { return id > other.id; }
    bool operator<(const Usuario& other) const { return id < other.id; }
    bool operator==(const Usuario& other) const { return id == other.id; }
};

int main() {
    // Crear un B-Tree en disco para el archivo binario "usuarios.bin" con ORDER = 5
    BTree<Usuario, 5> userTree("usuarios.bin");

    // Insertar un usuario
    Usuario u1{101, "kevin_abarca", "hash_secret123"};
    userTree.insert(u1);


    Usuario updateU1{101, "kevin", "hash_secret123"};
    if (userTree.update(updateU1)) {
        std::cout << "Usuario 101 actualizado exitosamente en el archivo binario.\n";
    } else {
        std::cout << "No se encontró el usuario a actualizar.\n";
    }

    // Buscar un usuario
    Usuario query{101, "", ""};
    Usuario result;

    if (userTree.search(query, result)) {
        std::cout << "Usuario encontrado: " << result.username << std::endl;
    }

    return 0;
}