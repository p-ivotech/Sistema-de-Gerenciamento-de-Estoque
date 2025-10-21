#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <map>
#include <locale>
#include <algorithm>
#include <cstdlib>
#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>

char getch() {
    char buf = 0;
    struct termios old = {};
    if (tcgetattr(0, &old) < 0) perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    if (tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0) perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0) perror("tcsetattr ~ICANON");
    return buf;
}
#endif

using namespace std;

struct Produto {
    string id;
    string nome;
    int quantidade;
    double preco;
};

vector<Produto> estoque;
const string ARQUIVO_ESTOQUE = "estoque.txt";

void limparTela() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pausarTela() {
    cout << "\nPressione ENTER para continuar...";
    cin.ignore();
    cin.get();
    limparTela();
}

void salvarEstoque() {
    ofstream arquivo(ARQUIVO_ESTOQUE);
    for (const auto &p : estoque) {
        arquivo << p.id << ";" << p.nome << ";" << p.quantidade << ";" << p.preco << "\n";
    }
}

void carregarEstoque() {
    ifstream arquivo(ARQUIVO_ESTOQUE);
    if (!arquivo.is_open()) return;

    Produto p;
    string linha;
    while (getline(arquivo, linha)) {
        size_t pos1 = linha.find(';');
        size_t pos2 = linha.find(';', pos1 + 1);
        size_t pos3 = linha.find(';', pos2 + 1);

        if (pos1 == string::npos || pos2 == string::npos || pos3 == string::npos) {
            cerr << "Linha invalida no arquivo: " << linha << endl;
            continue;
        }

        try {
            p.id = linha.substr(0, pos1);
            p.nome = linha.substr(pos1 + 1, pos2 - pos1 - 1);
            p.quantidade = stoi(linha.substr(pos2 + 1, pos3 - pos2 - 1));
            p.preco = stod(linha.substr(pos3 + 1));
            estoque.push_back(p);
        } catch (const std::exception &e) {
            cerr << "Erro ao processar linha: " << linha << "\nMotivo: " << e.what() << endl;
        }
    }
    arquivo.close();
}

void exibirEstoque() {
    sort(estoque.begin(), estoque.end(), [](const Produto &a, const Produto &b) {
        return a.id < b.id;
    });

    double valorTotal = 0;
    int totalItens = 0;
    cout.imbue(locale(""));
    cout << "\n==================== ESTOQUE ATUAL ====================\n";
    cout << left << setw(10) << "ID" << setw(20) << "Nome" << setw(12) << "Quantidade"
         << setw(12) << "Preco (R$)" << setw(15) << "Total (R$)" << endl;
    cout << "--------------------------------------------------------\n";
    for (const auto &p : estoque) {
        double totalProduto = p.quantidade * p.preco;
        cout << left << setw(10) << p.id << setw(20) << p.nome.substr(0, 18) << setw(12)
             << p.quantidade << fixed << setprecision(2) << setw(12) << p.preco
             << setw(15) << totalProduto << endl;
        valorTotal += totalProduto;
        totalItens += p.quantidade;
    }
    cout << "--------------------------------------------------------\n";
    cout << "Total de itens armazenados: " << totalItens << "\n";
    cout << "Valor total do estoque: R$ " << fixed << setprecision(2) << valorTotal << "\n";
}

void exportarCSV() {
    const char* userProfile = getenv("USERPROFILE");
    string caminhoCSV = string(userProfile) + "\\Documents\\estoque.csv";

    ofstream arquivo(caminhoCSV);
    if (!arquivo) {
        cout << "Erro ao criar o arquivo CSV em " << caminhoCSV << endl;
        return;
    }

    arquivo << "ID,Nome,Quantidade,Preco (R$),Total (R$)\n";
    for (const auto &p : estoque) {
        double totalProduto = p.quantidade * p.preco;
        arquivo << p.id << "," << p.nome << "," << p.quantidade << ","
                << fixed << setprecision(2) << p.preco << "," << totalProduto << "\n";
    }

    arquivo.close();
    cout << "Estoque exportado para '" << caminhoCSV << "' com sucesso!\n";
    exibirEstoque();
    pausarTela();
}

void adicionarProduto() {
    Produto p;
    cout << "ID do Produto: ";
    cin >> p.id;
    for (const auto &produto : estoque) {
        if (produto.id == p.id) {
            cout << "Produto com esse ID ja existe!\n";
            pausarTela();
            return;
        }
    }
    cin.ignore();
    cout << "Nome do Produto: ";
    getline(cin, p.nome);
    cout << "Quantidade Inicial: ";
    cin >> p.quantidade;
    cout << "Preco Unitario: ";
    cin >> p.preco;
    estoque.push_back(p);
    salvarEstoque();
    exibirEstoque();
    pausarTela();
}

void entradaProduto() {
    string id;
    int qtd;
    cout << "ID do Produto: ";
    cin >> id;
    cout << "Quantidade de Entrada: ";
    cin >> qtd;
    for (auto &p : estoque) {
        if (p.id == id) {
            p.quantidade += qtd;
            salvarEstoque();
            exibirEstoque();
            pausarTela();
            return;
        }
    }
    cout << "Produto nao encontrado!" << endl;
    exibirEstoque();
    pausarTela();
}

void saidaProduto() {
    string id;
    int qtd;
    cout << "ID do Produto: ";
    cin >> id;
    cout << "Quantidade de Saida: ";
    cin >> qtd;
    for (auto &p : estoque) {
        if (p.id == id) {
            if (p.quantidade >= qtd) {
                p.quantidade -= qtd;
                salvarEstoque();
                exibirEstoque();
            } else {
                cout << "Estoque insuficiente!" << endl;
            }
            exibirEstoque();
            pausarTela();
            return;
        }
    }
    cout << "Produto nao encontrado!" << endl;
    exibirEstoque();
    pausarTela();
}

void pesquisarProduto() {
    string id;
    cout << "ID do Produto: ";
    cin >> id;
    for (const auto &p : estoque) {
        if (p.id == id) {
            double total = p.quantidade * p.preco;
            cout << "\n==================== PRODUTO ENCONTRADO ====================\n";
            cout << "Nome: " << p.nome << "\n";
            cout << "Quantidade disponivel: " << p.quantidade << "\n";
            cout << "Preco unitario: R$ " << fixed << setprecision(2) << p.preco << "\n";
            cout << "Valor total: R$ " << total << "\n";
            exibirEstoque();
            pausarTela();
            return;
        }
    }
    cout << "Produto nao encontrado!\n";
    exibirEstoque();
    pausarTela();
}

void coracaoEspecial() {
    cout << R"(
     EU TE AMO!!
     ***     ***
   ******* *******
  ***************
   *************
    ***********
     *********
      *******
       *****
        ***
         *
)" << endl;
    exibirEstoque();
    pausarTela();
}

bool login(string &usuarioLogado) {
    map<string, string> usuarios = {
        {"admin", "1234"},
        {"user1", "senha1"},
        {"user2", "senha2"},
        {"michele", "1212"}
    };
    string usuario, senha;
    cout << "Usuario: ";
    cin >> usuario;
    cout << "Senha: ";
    char ch;
    senha = "";
    while ((ch = getch()) != '\r') {
        if (ch == '\b' && !senha.empty()) {
            senha.pop_back();
            cout << "\b \b";
        } else if (isprint(ch)) {
            senha += ch;
            cout << '*';
        }
    }
    cout << endl;

    if (usuarios.count(usuario) && usuarios[usuario] == senha) {
        usuarioLogado = usuario;
        cout << "\nLogin bem-sucedido!\n";
        return true;
    }
    cout << "\nUsuario ou senha incorretos!\n";
    return false;
}

int main() {
    locale::global(locale(""));
    string usuario;
    if (!login(usuario)) return 0;

    int opcao;
    carregarEstoque();
    exibirEstoque();
    do {
        cout << "\n=======================================================\n";
        cout << "                 SISTEMA DE ESTOQUE                   \n";
        cout << "=======================================================\n";
        cout << "[1] Adicionar Produto\n";
        cout << "[2] Entrada de Produto\n";
        cout << "[3] Saida de Produto\n";
        cout << "[4] Pesquisar Produto\n";
        cout << "[5] Exportar para CSV\n";
        cout << "[6] Ver Estoque\n";
        if (usuario == "michele")
            cout << "[8] Coracao Especial\n";
        cout << "[7] Sair\n";
        cout << "-------------------------------------------------------\n";
        cout << "Escolha uma Opcao: ";
        cin >> opcao;
        cout << "\n";

        switch (opcao) {
            case 1: adicionarProduto(); break;
            case 2: entradaProduto(); break;
            case 3: saidaProduto(); break;
            case 4: pesquisarProduto(); break;
            case 5: exportarCSV(); break;
            case 6: exibirEstoque(); pausarTela(); break;
            case 7: cout << "Saindo..." << endl; break;
            case 8: if (usuario == "michele") coracaoEspecial(); else cout << "Op��o inv�lida!\n"; break;
            default: cout << "Opcao invalida!\n";
        }
    } while (opcao != 7);

    return 0;
}
