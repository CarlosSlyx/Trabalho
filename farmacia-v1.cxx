#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

const int MAX_PRODUTOS = 10;

class MateriaPrima {
protected:
    string nome;
    int quantidadeMg;
    int quantidadeProdutos;
    double preco;

public:
    MateriaPrima() : nome(""), quantidadeMg(0), quantidadeProdutos(0), preco(0.0) {}

    MateriaPrima(string nome, int quantidadeMg, int quantidadeProdutos, double preco)
        : nome(nome), quantidadeMg(quantidadeMg), quantidadeProdutos(quantidadeProdutos), preco(preco) {}

    string getNome() const { return nome; }
    int getQuantidadeMg() const { return quantidadeMg; }
    int getQuantidadeProdutos() const { return quantidadeProdutos; }
    double getPreco() const { return preco; }

    void setMateriaPrima(string nome, int quantidadeMg, int quantidadeProdutos, double preco) {
        this->nome = nome;
        this->quantidadeMg = quantidadeMg;
        this->quantidadeProdutos = quantidadeProdutos;
        this->preco = preco;
    }

    void reduzirQuantidade(int qtde) {
        if (quantidadeProdutos >= qtde) {
            quantidadeProdutos -= qtde;
        }
    }

    bool estaDisponivel(int qtde) {
        return quantidadeProdutos >= qtde;
    }

    virtual string toString() const {
        stringstream ss;
        ss << nome << " " << quantidadeMg << "mg " << quantidadeProdutos << " unidades R$ " << fixed << setprecision(2) << preco;
        return ss.str();
    }
};

class Componente {
private:
    string nome;
    int quantidadeMg;

public:
    Componente() : nome(""), quantidadeMg(0) {}
    Componente(string nome, int quantidadeMg) : nome(nome), quantidadeMg(quantidadeMg) {}

    string getNome() const { return nome; }
    int getQuantidadeMg() const { return quantidadeMg; }
};

class ProdutoFinal : public MateriaPrima {
private:
    vector<Componente> componentes;

public:
    ProdutoFinal() : MateriaPrima() {}
    ProdutoFinal(string nome, int quantidadeMg, int quantidadeProdutos, double preco)
        : MateriaPrima(nome, quantidadeMg, quantidadeProdutos, preco) {}

    void adicionarComponente(Componente componente) {
        componentes.push_back(componente);
    }

    string toString() const {
        stringstream ss;
        ss << nome << " " << quantidadeMg << "mg " << quantidadeProdutos << " unidades R$ " << fixed << setprecision(2) << preco;

        if (!componentes.empty()) {
            ss << " - Composto por: ";
            for (const auto &comp : componentes) {
                ss << comp.getQuantidadeMg() << "mg de " << comp.getNome() << " + ";
            }
          
            string s = ss.str();
            s = s.substr(0, s.length() - 3);
            return s;
        }

        return ss.str();
    }
};

class Cliente {
private:
    string nome;
    int idade;
    string cpf;

public:
    Cliente() : nome(""), idade(0), cpf("") {}
    Cliente(string nome, int idade, string cpf) : nome(nome), idade(idade), cpf(cpf) {}

    string getNome() const { return nome; }
    int getIdade() const { return idade; }
    string getCpf() const { return cpf; }

    void salvarRecibo(const string &nomeProdutoFinal, int quantidadeComponentes, double total) const {
        string nomeArquivo = nome + "-recibo.txt";
        ofstream arquivo(nomeArquivo);
        if (arquivo.is_open()) {
            arquivo << "Farmacia Universitaria" << endl;
            arquivo << "Nome: " << nome << endl;
            arquivo << "Idade: " << idade << endl;
            arquivo << "CPF: " << cpf << endl;
            arquivo << "Itens Comprados:" << endl;
            arquivo << quantidadeComponentes << "x " << nomeProdutoFinal << " - Valor total: R$ " << fixed << setprecision(2) << total << endl;
            arquivo.close();
            cout << "Recibo gerado com sucesso!" << endl;
        } else {
            cerr << "Erro ao criar arquivo de recibo." << endl;
        }
    }
};

class Estoque {
private:
    ProdutoFinal produtos[MAX_PRODUTOS];
    int numProdutos;

public:
    Estoque() : numProdutos(0) {}

    bool carregarEstoque() {
        ifstream arquivo("estoque.txt");
        if (!arquivo) {
            return false;
        }

        string nome;
        int quantidadeMg;
        int quantidadeProdutos;
        double preco;

        while (arquivo >> nome >> quantidadeMg >> quantidadeProdutos >> preco) {
            ProdutoFinal produto(nome, quantidadeMg, quantidadeProdutos, preco);
            adicionarProduto(produto);
        }

        arquivo.close();
        return true;
    }

    void salvarEstoque() const {
        ofstream arquivo("estoque.txt");
        if (arquivo.is_open()) {
            for (int i = 0; i < numProdutos; ++i) {
                arquivo << produtos[i].getNome() << " " <<
                    produtos[i].getQuantidadeMg() << " " <<
                    produtos[i].getQuantidadeProdutos() << " " <<
                    fixed << setprecision(2) << produtos[i].getPreco() << endl;
            }
            arquivo.close();
        } else {
            cerr << "Erro ao salvar estoque." << endl;
        }
    }

    void adicionarProduto(ProdutoFinal produto) {
        for (int i = 0; i < numProdutos; ++i) {
            if (produtos[i].getNome() == produto.getNome()) {
                produtos[i].setMateriaPrima(produto.getNome(), produto.getQuantidadeMg(),
                    produtos[i].getQuantidadeProdutos() + produto.getQuantidadeProdutos(),
                    produto.getPreco());
                return;
            }
        }

        if (numProdutos < MAX_PRODUTOS) {
            produtos[numProdutos++] = produto;
        } else {
            cout << "Estoque cheio. Nao e possivel adicionar mais produtos." << endl;
        }
    }

    void imprimirEstoque() const {
        if (numProdutos == 0) {
            cout << "Estoque vazio." << endl;
        } else {
            for (int i = 0; i < numProdutos; ++i) {
                cout << i + 1 << ". " << produtos[i].toString() << endl;
            }
        }
    }

    bool abaterEstoque(const string &nomeProd, int quantidade) {
        for (int i = 0; i < numProdutos; ++i) {
            if (produtos[i].getNome() == nomeProd) {
                if (produtos[i].estaDisponivel(quantidade)) {
                    produtos[i].reduzirQuantidade(quantidade);
                    cout << "O produto foi comprado com sucesso !!" << endl;
                    return true;
                } else {
                    cout << "Quantidade solicitada nao disponivel em estoque." << endl;
                    return false;
                }
            }
        }
        cout << "Produto nao encontrado no estoque." << endl;
        return false;
    }

    ProdutoFinal *obterProduto(const string &nomeProd) {
        for (int i = 0; i < numProdutos; ++i) {
            if (produtos[i].getNome() == nomeProd) {
                return &produtos[i];
            }
        }
        return nullptr;
    }
};

int main() {
    int escolha;
    Estoque estoque;
    
    ofstream arquivo("estoque.txt");

    if (!estoque.carregarEstoque()) {
        cerr << "Erro ao carregar estoque. Iniciando com estoque vazio." << endl;
    }

    while (true) {
        cout << "\nMenu:" << endl;
        cout << "[1] Cadastrar Materia Prima" << endl;
        cout << "[2] Comprar Produto" << endl;
        cout << "[3] Listar Estoque" << endl;
        cout << "[4] Sair" << endl;
        cout << "Escolha uma opcao: ";
        cin >> escolha;

        switch (escolha) {
        case 1: {
            char rep = 'S';
            while (rep == 'S' || rep == 's') {
                string nome;
                int quantidadeMg;
                int quantidadeProdutos;
                double preco;

                cout << "\nCadastro de Materia Prima" << endl;
                cout << "Nome: ";
                cin.ignore();
                getline(cin, nome);
                cout << "Quantidade em mg: ";
                cin >> quantidadeMg;
                cout << "Quantidade de produtos: ";
                cin >> quantidadeProdutos;
                cout << "Preco: ";
                cin >> preco;

                ProdutoFinal produto(nome, quantidadeMg, quantidadeProdutos, preco);
                estoque.adicionarProduto(produto);
                estoque.salvarEstoque();
                cout << "Materia Prima cadastrada com sucesso!" << endl;
                cout << endl;
                cout << "Deseja adicionar mais uma materia prima? [S/N]" << endl;
                cin >> rep;
            }
            break;
        }
        case 2: {
            vector<pair<ProdutoFinal*, int>> produtosSelecionados;

            while (true) {
                cout << "\nLista de Produtos no Estoque:" << endl;
                cout << endl;
                estoque.imprimirEstoque();

                cout << "\nCompra de Produto" << endl;
                cout << "Nome do Produto ('0' para encerrar a selecao): ";
                string nomeProd;
                cin.ignore();
                getline(cin, nomeProd);

                if (nomeProd == "0") {
                    break;
                }

                ProdutoFinal *produto = estoque.obterProduto(nomeProd);
                if (produto == nullptr) {
                    cout << "Produto nao encontrado." << endl;
                    continue;
                }

                cout << "Quantidade desejada: ";
                int quantidade;
                cin >> quantidade;

                if (!estoque.abaterEstoque(nomeProd, quantidade)) {
                    continue;
                }

                produtosSelecionados.push_back(make_pair(produto, quantidade));
            }

            if (produtosSelecionados.empty()) {
                cout << "Nenhum produto selecionado." << endl;
                break;
            }

            double total = 0.0;
            int totalMg = 0;
            stringstream nomeProdutoFinal;

            ProdutoFinal produtoFinal;
            for (auto &entry : produtosSelecionados) {
                ProdutoFinal* prod = entry.first;
                int quantidade = entry.second;

                double precoProduto = prod->getPreco();
                total += precoProduto * quantidade;
                totalMg += prod->getQuantidadeMg() * quantidade;

                Componente comp(prod->getNome(), quantidade * prod->getQuantidadeMg());
                produtoFinal.adicionarComponente(comp);

                nomeProdutoFinal << prod->getNome() << " " << quantidade * prod->getQuantidadeMg() << "mg" << " + ";
            }

            string resumoCompra = nomeProdutoFinal.str();
            resumoCompra = resumoCompra.substr(0, resumoCompra.length() - 3);

            produtoFinal.setMateriaPrima(resumoCompra, totalMg, 1, total);

            cout << "\nResumo da Compra:" << endl;
            cout << "Produto Final: " << produtoFinal.toString() << endl;
            cout << "Valor total: R$ " << fixed << setprecision(2) << total << endl;

            char confirmacao;
            cout << "\nDeseja prosseguir com a compra? [S/N]: ";
            cin >> confirmacao;

            if (confirmacao == 'S' || confirmacao == 's') {
                string nome;
                int idade;
                string cpf;

                cout << "\nCompra concluida. Por favor, forneça seus dados:" << endl;
                cout << "Nome: ";
                cin.ignore();
                getline(cin, nome);
                cout << "Idade: ";
                cin >> idade;
                cout << "CPF: ";
                cin.ignore();
                getline(cin, cpf);

                Cliente cliente(nome, idade, cpf);
                cliente.salvarRecibo(resumoCompra, 1, total);
            }

            break;
        }
        case 3:
            cout << "\nLista de Produtos no Estoque:" << endl;
            cout << endl;
            estoque.imprimirEstoque();
            break;
        case 4:
            cout << "Saindo do programa..." << endl;
            return 0;
        default:
            cout << "Opcao invalida. Tente novamente." << endl;
            break;
        }
    }

    return 0;
}
