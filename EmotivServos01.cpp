/**
  *
  * Aplicativo desenvolvido em C/C++ utilizando o headset
  * Emotiv Epoc em conjunto com ESP32 para manipular
  * seis servo motores (prótese de antebraço feita com impressão 3D).
  *
  * Data de criação:                11/07/2019
  * Atualizado pela última vez em:  19/02/2020
  *
  * Autor: João Pedro Mayrink de Jesus
  * E-mail: joaopmayrinkj@gmail.com
  *
  * Epoc(TM) é marca registrada da Emotiv(R) Systems, Inc.
  *
  */

//Bibliotecas do Windows
#include <windows.h>
#include <iostream>

//Headers Emotiv
#include "EmoStateDLL.h"
#include "edk.h"
#include "edkErrorCode.h"

//Classe para comunicação serial.
#include "SerialClass.h"


//Constantes (Portas das aplicações)
const unsigned int CONTROL_PANEL_PORT = 3008;
const unsigned int EMO_COMPOSER_PORT = 1726;

//Biblioteca Emotiv
#pragma comment(lib, "./lib/edk.lib")

//Namespace STL (C++)
using namespace std;

typedef enum options {
    EMPTY,
    HELP,
    CONECTAR,
    DESCONECTAR,
    LER_EMO_STATE,
    EXIT,
    USERERROR
} options;

class User {
private:
    boolean connectionStatus = false;
    int connectionType = 0; // CONTROL_PANEL_PORT ou EMO_COMPOSER_PORT

public:
    void connect(int connection) {
        connectionStatus = true;
        connectionType = connection;
    }
    void disconnect(void) {
        connectionStatus = false;
        connectionType = 0;
    }

    boolean isConnected(void) {
        return connectionStatus;
    }

    int getConnectionType(void) {
        return connectionType;
    }

    void setConnectionType(int connection) {
        connectionType = connection;
    }
};

//Protótipos de funções
void startConnection(void);
void endConnection(void);
void showCommands(void);
int converte(string);
int requestUser(void);
void finishProgram(void);
void promptUser(void);
void invalidCommand(void);
void showEmoState(void);


User user;

int main(int argc, char** argv) {
	cout << "\nInicializando aplicacao..." << endl;
    cout << "\nBem-vindo(a)! Digite um comando, ou digite 'help' para uma lista dos comandos disponiveis" << endl;
    while (1) {
        int operation;
        operation = requestUser();
        switch (operation) {
        case EMPTY:
            promptUser();
            break;
        case HELP:
            showCommands();
            break;
        case CONECTAR:
            startConnection();
            break;
        case DESCONECTAR:
            endConnection();
            break;
        case LER_EMO_STATE:
            showEmoState();
            break;
        case EXIT:
            finishProgram();
            break;
        case USERERROR:
            invalidCommand();
            break;
        }
    }
}

int requestUser(void) {
    string option;
    promptUser();
    cin >> option;
    int saida = converte(option);
    return saida;
}

void promptUser(void) {
    cout << "EmotivServos01> ";
}

int converte(string entrada) {
    if (entrada == "")
        return EMPTY;
    else {
        if (entrada == "help")
           return HELP;
        else {
            if (entrada == "conectar")
                return CONECTAR;
            else {
                if (entrada == "desconectar")
                    return DESCONECTAR;
                else {
                    if (entrada == "ler")
                        return LER_EMO_STATE;
                    else {
                        if (entrada == "exit")
                            return EXIT;
                        else
                            return USERERROR;
                    }
                }
            }
        }
    }
}

void showCommands(void) {
    cout << "\nCOMANDOS" << endl;
    cout << "help................mostra uma lista dos comandos disponiveis" << endl;
    cout << "conectar............inicia a conexao com o headset" << endl;
    cout << "desconectar.........finaliza a conexao existente" << endl;
    cout << "ler.................exibe o valor do EmoState acumulado na pilha" << endl;
    cout << "exit................encerra o programa\n" << endl;
}

void startConnection(void) { // implementar teste se a conexão já estiver sido feita

    if (user.isConnected()) {
        int connectionInt = user.getConnectionType();
        string connectionStr;
        if (connectionInt == EMO_COMPOSER_PORT)
            connectionStr = "Emo Composer";
        if (connectionInt == CONTROL_PANEL_PORT)
            connectionStr = "Painel de Controle";
        cout << "\nVoce ja esta conectado a aplicacao " << connectionStr << endl;
        return;
    }

    else {
        cout << "\nA qual aplicacao vc deseja se conectar?" << endl;
        cout << "PAINEL DE CONTROLE ......... 0" << endl;
        cout << "EMO COMPOSER ............... 1" << endl;
        promptUser();
        int option = 0;
        cin >> option;
        switch (option) {
        case 0:
            cout << "\nIniciando tentativa de conexao com o Emo Engine..." << endl;
            if ((EE_EngineRemoteConnect("127.0.0.1", CONTROL_PANEL_PORT) == EDK_OK)) {
                cout << "Conectado com sucesso ao Painel de Controle." << endl;
                user.connect(CONTROL_PANEL_PORT);
            }
            else {
                cout << "Falha na conexao. Certifique-se que o Painel de Controle esta ativo e tente novamente" << endl;
            }
            break;
        case 1:
            cout << "\nIniciando tentativa de conexao com o Emo Engine..." << endl;
            if ((EE_EngineRemoteConnect("127.0.0.1", EMO_COMPOSER_PORT) == EDK_OK)) {
                cout << "Conectado com sucesso ao Emo Composer." << endl;
                user.connect(EMO_COMPOSER_PORT);
            }
            else {
                cout << "\nFalha na conexao. Certifique-se que o Emo Composer esta ativo e tente novamente" << endl;
            }
            break;
        default:
            invalidCommand();
            break;
        }
    }
}

void endConnection(void) { // implementar teste se a conexão já tiver sido desfeita
    cout << "\nDesconectando do Emo Engine..." << endl;
    if (EE_EngineDisconnect() == EDK_OK) { // desconecta do EmoEngine, e retorna código de erro
        cout << "Desconectado com sucesso" << endl;
        user.disconnect();
    }
    else
        cout << "\nErro ao desconectar. Tente novamente." << endl;
}

void showEmoState(void) {
    EmoEngineEventHandle eEvent   = EE_EmoEngineEventCreate();
    EmoStateHandle       eState   = EE_EmoStateCreate();
    int                  newEvent = EE_EngineGetNextEvent(eEvent);
    for (int i = 0; i < 10; i++)
        cout << newEvent << " ";

    cout << endl;
    newEvent = EE_EmoEngineEventGetEmoState(eEvent, eState);

    for (int i = 0; i < 10; i++)
        cout << newEvent << " ";
    cout << endl;
}

void finishProgram(void) {
    endConnection();
    cout << "\nFinalizando aplicacao. Ate mais!" << endl << endl << endl;
    exit(EXIT_SUCCESS); // argumento 0 (EXIT_SUCCESS) -> programa encerrado corretamente // argumento 1 (EXIT_FAILURE) -> programa encerrado abruptamente (informado ao S.O.)
}

void invalidCommand(void) {
    cout << "\nComando invalido. Digite 'help' para uma lista dos comandos disponiveis" << endl;
}
