#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <vector>

#include"types.h"

using namespace std;

class patriciaTree {
private:
    struct Node {
        TF is_end_of_word = false;
        TI frequency = 0;
        std::map<std::string, std::unique_ptr<Node>> children;
    };

    std::unique_ptr<Node> root;

    // Calcula la longitud del prefijo común más largo entre dos cadenas
    size_t getCommonPrefixLength(const std::string& s1, const std::string& s2) const {
        size_t len = 0;
        while (len < s1.length() && len < s2.length() && s1[len] == s2[len]) 
            len++;
        return len;
    }

    // Inserción con división de nodos (Split) en tiempo de ejecución
    void insertHelper(Node* curr, const std::string& word) {
        for (auto it = curr->children.begin(); it != curr->children.end(); ++it) {
            const std::string& edge = it->first;
            size_t common_len = getCommonPrefixLength(edge, word);

            if (common_len > 0) {
                std::string common_prefix = edge.substr(0, common_len);
                std::string remaining_edge = edge.substr(common_len);
                std::string remaining_word = word.substr(common_len);

                // Caso 1: El prefijo coincide totalmente con la arista existente
                if (remaining_edge.empty()) {
                    if (remaining_word.empty()) {
                        it->second->is_end_of_word = true;
                        it->second->frequency++;
                    } else {
                        insertHelper(it->second.get(), remaining_word);
                    }
                    return;
                }

                // Caso 2: Bifurcación (Split). Se debe romper la arista existente.
                auto child_node = std::move(it->second);
                curr->children.erase(it);

                auto split_node = std::make_unique<Node>();
                split_node->children[remaining_edge] = std::move(child_node);

                if (remaining_word.empty()) {
                    split_node->is_end_of_word = true;
                    split_node->frequency = 1;
                } else {
                    auto new_leaf = std::make_unique<Node>();
                    new_leaf->is_end_of_word = true;
                    new_leaf->frequency = 1;
                    split_node->children[remaining_word] = std::move(new_leaf);
                }

                curr->children[common_prefix] = std::move(split_node);
                return;
            }
        }

        // Caso 3: No hay prefijo común, se crea una rama nueva
        auto new_node = std::make_unique<Node>();
        new_node->is_end_of_word = true;
        new_node->frequency = 1;
        curr->children[word] = std::move(new_node);
    }

    // Impresor recursivo adaptado al formato de directorios solicitado
    void printHelper(const Node* node, const std::string& indent, TF isLast, const std::string& edge_label) const {
        if (!edge_label.empty()) {
            std::cout << indent;
            if (isLast) 
                std::cout << "\\-- " << edge_label;
            else 
                std::cout << "|-- " << edge_label;
            if (node->is_end_of_word) {
                std::cout << "*";
                if (node->frequency > 1) 
                    std::cout << " (" << node->frequency << ")";
            }
            std::cout << "\n";
        }

        // El indentado se propaga acumulando tuberías '|' o espacios vacíos
        std::string newIndent = indent;
        if (!edge_label.empty()) 
            newIndent += (isLast ? "    " : "|   ");
        size_t childCount = node->children.size();
        size_t i = 0;
        for (const auto& [edge, child] : node->children) {
            TF childLast = (i == childCount - 1);
            printHelper(child.get(), newIndent, childLast, edge);
            i++;
        }
    }

public:
    patriciaTree() : root(std::make_unique<Node>()) {}

    void insert(const std::string& word) {
        if (word.empty()) return;
        insertHelper(root.get(), word);
    }

    void print() const {
        std::cout << "(root)\n";
        size_t childCount = root->children.size();
        size_t i = 0;
        for (const auto& [edge, child] : root->children) {
            TF childLast = (i == childCount - 1);
            printHelper(child.get(), "", childLast, edge);
            i++;
        }
    }
};

int main() {
    patriciaTree poem_trie;

    // "Los Heraldos Negros" completo (normalizado a minúsculas y sin tildes)
    std::vector<TS> words = {
        "hay", "golpes", "en", "la", "vida", "tan", "fuertes", "yo", "no", "se",
        "golpes", "como", "del", "odio", "de", "dios", "como", "si", "ante", "ellos",
        "la", "resaca", "de", "todo", "lo", "sufrido", "se", "empozara", "en", "el", "alma", "yo", "no", "se",
        "son", "pocos", "pero", "son", "abren", "zanjas", "oscuras", "en", "el", "rostro", "mas", "fiero", "y", "en", "el", "lomo", "mas", "fuerte",
        "seran", "tal", "vez", "los", "potros", "de", "barbaros", "atilas", "o", "los", "heraldos", "negros", "que", "nos", "manda", "la", "muerte",
        "son", "las", "caidas", "hondas", "de", "los", "cristos", "del", "alma", "de", "alguna", "fe", "adorable", "que", "el", "destino", "blasfema",
        "esos", "golpes", "sangrientos", "son", "las", "crepitaciones", "de", "algun", "pan", "que", "en", "la", "puerta", "del", "horno", "se", "nos", "quema",
        "y", "el", "hombre", "pobre", "pobre", "vuelve", "los", "ojos", "como", "cuando", "por", "sobre", "el", "hombro", "nos", "llama", "una", "palmada",
        "vuelve", "los", "ojos", "locos", "y", "todo", "lo", "vivido", "se", "empoza", "como", "charco", "de", "culpa", "en", "la", "mirada",
        "hay", "golpes", "en", "la", "vida", "tan", "fuertes", "yo", "no", "se"
    };

    for (const auto& word : words) {
        poem_trie.insert(word);
    }

    std::cout << "=== RADIX TREE (COMPRIMIDO): LOS HERALDOS NEGROS ===\n\n";
    poem_trie.print();

    return 0;
}