
#pragma once
#include <iosfwd>
#include "BTreeNode.hpp"
#include <fstream>
using namespace std;
struct BTreeHeader {
    uint64_t rootOffset;
    uint64_t nextFreeOffset;
};


// Clase BTree para Archivos Binarios
template <typename T, int ORDER>
class BTree {
private:
    std::fstream file;
    std::string filename;
    BTreeHeader header;

    //Métodos Auxiliares de E/S en Disco

    void readNode(uint64_t offset, BTreeNode<T, ORDER>& node) {
        file.seekg(offset, std::ios::beg);
        file.read(reinterpret_cast<char*>(&node), sizeof(BTreeNode<T, ORDER>));
    }

    void writeNode(uint64_t offset, const BTreeNode<T, ORDER>& node) {
        file.seekp(offset, std::ios::beg);
        file.write(reinterpret_cast<const char*>(&node), sizeof(BTreeNode<T, ORDER>));
        file.flush(); // Garantiza persistencia inmediata en disco
    }

    void readHeader() {
        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char*>(&header), sizeof(BTreeHeader));
    }

    void writeHeader() {
        file.seekp(0, std::ios::beg);
        file.write(reinterpret_cast<const char*>(&header), sizeof(BTreeHeader));
        file.flush();
    }

    uint64_t allocateNode(const BTreeNode<T, ORDER>& node) {
        uint64_t offset = header.nextFreeOffset;
        writeNode(offset, node);
        header.nextFreeOffset += sizeof(BTreeNode<T, ORDER>);
        writeHeader();
        return offset;
    }

    // --- Métodos Privados del B-Tree ---

    void splitChild(uint64_t parentOffset, BTreeNode<T, ORDER>& parentNode, int i, uint64_t childOffset) {
        BTreeNode<T, ORDER> yNode;
        readNode(childOffset, yNode);

        // Crear nuevo nodo z para almacenar la mitad derecha de yNode
        BTreeNode<T, ORDER> zNode(yNode.leaf);
        int mid = (ORDER - 1) / 2;

        zNode.n = (ORDER - 1) - mid - 1;

        // Copiar la segunda mitad de claves a zNode
        for (int j = 0; j < zNode.n; j++) {
            zNode.keys[j] = yNode.keys[mid + 1 + j];
        }

        // Copiar la segunda mitad de hijos a zNode (si no es hoja)
        if (!yNode.leaf) {
            for (int j = 0; j <= zNode.n; j++) {
                zNode.children[j] = yNode.children[mid + 1 + j];
            }
        }

        yNode.n = mid; // Reducir claves del nodo original yNode

        // Guardar zNode en una nueva posición libre del archivo
        uint64_t zOffset = allocateNode(zNode);

        // Desplazar hijos en el padre para insertar el nuevo offset zOffset
        for (int j = parentNode.n; j >= i + 1; j--) {
            parentNode.children[j + 1] = parentNode.children[j];
        }
        parentNode.children[i + 1] = zOffset;

        // Desplazar claves en el padre para promocionar la clave mediana
        for (int j = parentNode.n - 1; j >= i; j--) {
            parentNode.keys[j + 1] = parentNode.keys[j];
        }
        parentNode.keys[i] = yNode.keys[mid];
        parentNode.n++;

        // Escribir los tres nodos actualizados a disco
        writeNode(childOffset, yNode);
        writeNode(parentOffset, parentNode);
    }

    void insertNonFull(uint64_t nodeOffset, BTreeNode<T, ORDER>& node, const T& key) {
        int i = node.n - 1;

        if (node.leaf) {
            while (i >= 0 && node.keys[i] > key) {
                node.keys[i + 1] = node.keys[i];
                i--;
            }
            node.keys[i + 1] = key;
            node.n++;
            writeNode(nodeOffset, node);
        } else {
            while (i >= 0 && node.keys[i] > key) {
                i--;
            }
            i++;

            BTreeNode<T, ORDER> childNode;
            readNode(node.children[i], childNode);

            if (childNode.n == ORDER - 1) { // Si el hijo está lleno, dividirlo
                splitChild(nodeOffset, node, i, node.children[i]);
                if (node.keys[i] < key) {
                    i++;
                }
            }

            readNode(node.children[i], childNode);
            insertNonFull(node.children[i], childNode, key);
        }
    }

public:
    BTree(const std::string& dbPath) : filename(dbPath) {
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);

        if (!file.is_open()) {
            // Si el archivo no existe, crearlo e inicializar el encabezado
            file.clear();
            file.open(filename, std::ios::out | std::ios::binary);
            file.close();

            file.open(filename, std::ios::in | std::ios::out | std::ios::binary);

            header.rootOffset = DISK_NULL;
            header.nextFreeOffset = sizeof(BTreeHeader);
            writeHeader();
        } else {
            readHeader();
        }
    }

    ~BTree() {
        if (file.is_open()) {
            file.close();
        }
    }

    void insert(const T& key) {
        if (header.rootOffset == DISK_NULL) {
            // El árbol está vacío: crear el primer nodo raíz
            BTreeNode<T, ORDER> rootNode(true);
            rootNode.keys[0] = key;
            rootNode.n = 1;

            header.rootOffset = allocateNode(rootNode);
            writeHeader();
        } else {
            BTreeNode<T, ORDER> rootNode;
            readNode(header.rootOffset, rootNode);

            if (rootNode.n == ORDER - 1) { // Si la raíz está llena
                BTreeNode<T, ORDER> newRoot(false);
                newRoot.children[0] = header.rootOffset;

                uint64_t newRootOffset = allocateNode(newRoot);
                header.rootOffset = newRootOffset;
                writeHeader();

                splitChild(newRootOffset, newRoot, 0, newRoot.children[0]);
                insertNonFull(newRootOffset, newRoot, key);
            } else {
                insertNonFull(header.rootOffset, rootNode, key);
            }
        }
    }

    bool search(const T& key, T& result) {
        if (header.rootOffset == DISK_NULL) return false;

        uint64_t currentOffset = header.rootOffset;
        while (currentOffset != DISK_NULL) {
            BTreeNode<T, ORDER> node;
            readNode(currentOffset, node);

            int i = 0;
            while (i < node.n && key > node.keys[i]) {
                i++;
            }

            if (i < node.n && node.keys[i] == key) {
                result = node.keys[i]; // Clave encontrada
                return true;
            }

            if (node.leaf) break;
            currentOffset = node.children[i];
        }
        return false;
    }


    bool update(const T& updatedRecord) {
        if (header.rootOffset == DISK_NULL) return false;

        uint64_t currentOffset = header.rootOffset;

        // Recorremos el árbol desde la raíz
        while (currentOffset != DISK_NULL) {
            BTreeNode<T, ORDER> node;
            readNode(currentOffset, node);

            int i = 0;
            // Buscar la posición correspondiente dentro del nodo actual
            while (i < node.n && updatedRecord > node.keys[i]) {
                i++;
            }

            // Si la clave existe en el nodo actual
            if (i < node.n && node.keys[i] == updatedRecord) {
                // 1. Actualizamos el registro en la RAM del nodo
                node.keys[i] = updatedRecord;

                // 2. Escribimos el nodo modificado de vuelta en el offset exacto en disco
                writeNode(currentOffset, node);

                return true; // Actualización exitosa
            }

            // Si es hoja y no lo encontramos, el registro no existe
            if (node.leaf) break;

            // Si no está en este nodo interno, descendemos al hijo correspondiente
            currentOffset = node.children[i];
        }

        return false; // Registro no encontrado
    }
};