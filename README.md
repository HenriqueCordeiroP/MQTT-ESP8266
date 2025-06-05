# 🚶‍♂️ Monitor de Fluxo de Pessoas

Um sistema IoT para monitoramento de fluxo de pessoas em tempo real utilizando ESP8266, Raspberry Pi, MQTT, Zabbix e Grafana.

## 📋 Descrição do Projeto

Este projeto implementa uma solução completa de monitoramento de fluxo de pessoas utilizando tecnologias IoT. O sistema coleta dados através de sensores conectados a um microcontrolador ESP8266, transmite as informações via protocolo MQTT para uma Raspberry Pi que atua como broker, e visualiza os dados em tempo real através de dashboards no Grafana.

### 🏗️ Arquitetura do Sistema

```
┌─────────────────┐    WiFi/MQTT    ┌─────────────────┐
│   ESP8266       │ ───────────────► │  Raspberry Pi 3 │
│ + Sensores      │                  │ + Mosquitto     │
│ + LEDs          │                  │ + Zabbix Agent  │
└─────────────────┘                  └─────────────────┘
                                              │
                                              │ Zabbix Protocol
                                              ▼
                                     ┌─────────────────┐
                                     │ Computador      │
                                     │ + Zabbix Server │
                                     │ + Grafana       │
                                     └─────────────────┘
```

### ✨ Funcionalidades

- **Detecção de Fluxo**: Monitora entrada e saída de pessoas através de sensores
- **Comunicação MQTT**: Transmissão de dados em tempo real via protocolo MQTT
- **Visualização**: Dashboards interativos no Grafana
- **Monitoramento**: Integração com Zabbix para alertas e métricas
- **Feedback Visual**: LEDs indicam eventos de entrada e saída

## 🔧 Requisitos

### Hardware

- 1x Raspberry Pi 3 (ou superior)
- 1x ESP8266 (NodeMCU, Wemos D1 Mini, etc.)
- 2x Push-down buttons
- 2x LEDs (vermelho e verde recomendados)
- 2x Resistores (220Ω para LEDs)
- 7x Jumper cables
- 1x Protoboard
- Fonte de alimentação para Raspberry Pi
- Cabo micro-USB para ESP8266

### Software

#### Raspberry Pi

- Raspbian OS (ou Ubuntu para ARM)
- Mosquitto MQTT Broker
- Zabbix Agent
- Python 3.x

#### Computador de Monitoramento

- Zabbix Server
- Grafana
- Sistema operacional compatível (Linux, Windows, macOS)

#### ESP8266

- Arduino IDE
- Bibliotecas: ESP8266WiFi, PubSubClient

### Rede

- Rede Wi-Fi 2.4GHz
- Acesso à internet (opcional, para atualizações)

## 🚀 Instruções de Instalação

### 1. Configuração da Raspberry Pi

#### 1.1 Preparação do Sistema

```bash
sudo apt update && sudo apt upgrade -y
sudo apt install git python3 python3-pip -y
```

#### 1.2 Instalação do Mosquitto MQTT Broker

```bash
sudo apt install mosquitto mosquitto-clients -y
sudo systemctl enable mosquitto
sudo systemctl start mosquitto
```

#### 1.3 Configuração do Mosquitto

```bash
sudo nano /etc/mosquitto/mosquitto.conf
```

Adicione as seguintes linhas:

```
listener 1883
allow_anonymous true
persistence true
persistence_location /var/lib/mosquitto/
log_dest file /var/log/mosquitto/mosquitto.log
```

Reinicie o serviço:

```bash
sudo systemctl restart mosquitto
```

#### 1.4 Instalação do Zabbix Agent

```bash
wget https://repo.zabbix.com/zabbix/6.0/raspbian/pool/main/z/zabbix-release/zabbix-release_6.0-4+debian11_all.deb
sudo dpkg -i zabbix-release_6.0-4+debian11_all.deb
sudo apt update
sudo apt install zabbix-agent -y
```

#### 1.5 Configuração do Zabbix Agent

```bash
sudo nano /etc/zabbix/zabbix_agentd.conf
```

Modifique as seguintes linhas:

```
Server=SEU_IP_DO_COMPUTADOR
ServerActive=SEU_IP_DO_COMPUTADOR
Hostname=raspberry-pi-monitor
UserParameter=mqtt.traffic.flow,mosquitto_sub -h localhost -t monitor/traffic-flow -C 1
```

Reinicie o agente:

```bash
sudo systemctl enable zabbix-agent
sudo systemctl restart zabbix-agent
```

### 2. Configuração do ESP8266

#### 2.1 Preparação do Arduino IDE

1. Instale o Arduino IDE
2. Adicione o board ESP8266:
   - Vá em File → Preferences
   - Em "Additional Board Manager URLs", adicione: `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
   - Vá em Tools → Board → Board Manager
   - Procure por "esp8266" e instale

#### 2.2 Instalação das Bibliotecas

No Arduino IDE, vá em Sketch → Include Library → Manage Libraries e instale:

- `PubSubClient` by Nick O'Leary
- `ESP8266WiFi` (já incluída no core ESP8266)

#### 2.3 Circuito de Conexão

```
ESP8266 Pin    Componente
D2         →   Button Enter (outro terminal ao GND)
D3         →   Button Exit (outro terminal ao GND)
D4         →   LED Enter + Resistor 220Ω → GND
D5         →   LED Exit + Resistor 220Ω → GND
3V3        →   VCC dos componentes (se necessário)
GND        →   GND comum
```

#### 2.4 Upload do Código

1. Copie o código do ESP8266 do apêndice do relatório
2. Modifique as variáveis de configuração:
   ```cpp
   const char* ssid = "SEU_WIFI_SSID";
   const char* password = "SUA_SENHA_WIFI";
   const char* mqtt_server = "IP_DA_RASPBERRY_PI";
   ```
3. Selecione a placa ESP8266 e a porta correta
4. Faça o upload do código

### 3. Configuração do Zabbix Server

#### 3.1 Instalação (Ubuntu/Debian)

```bash
wget https://repo.zabbix.com/zabbix/6.0/ubuntu/pool/main/z/zabbix-release/zabbix-release_6.0-4+ubuntu22.04_all.deb
sudo dpkg -i zabbix-release_6.0-4+ubuntu22.04_all.deb
sudo apt update
sudo apt install zabbix-server-mysql zabbix-frontend-php zabbix-apache-conf zabbix-sql-scripts zabbix-agent -y
```

#### 3.2 Configuração do Banco de Dados

```bash
sudo mysql -u root -p
mysql> create database zabbix character set utf8mb4 collate utf8mb4_bin;
mysql> create user zabbix@localhost identified by 'password';
mysql> grant all privileges on zabbix.* to zabbix@localhost;
mysql> quit;
```

Importe o esquema inicial:

```bash
zcat /usr/share/doc/zabbix-sql-scripts/mysql/server.sql.gz | mysql -uzabbix -p zabbix
```

#### 3.3 Configuração do Zabbix Server

```bash
sudo nano /etc/zabbix/zabbix_server.conf
```

Configure a conexão com o banco:

```
DBPassword=password
```

#### 3.4 Inicialização dos Serviços

```bash
sudo systemctl enable zabbix-server zabbix-agent apache2
sudo systemctl restart zabbix-server zabbix-agent apache2
```

### 4. Configuração do Grafana

#### 4.1 Instalação

```bash
sudo apt-get install -y adduser libfontconfig1
wget https://dl.grafana.com/enterprise/release/grafana-enterprise_10.0.0_amd64.deb
sudo dpkg -i grafana-enterprise_10.0.0_amd64.deb
```

#### 4.2 Inicialização

```bash
sudo systemctl enable grafana-server
sudo systemctl start grafana-server
```

#### 4.3 Configuração

1. Acesse http://localhost:3000
2. Login padrão: admin/admin
3. Configure o data source do Zabbix
4. Crie dashboards para visualizar os dados do fluxo de pessoas

## 📊 Uso do Sistema

### Testando a Comunicação MQTT

#### Na Raspberry Pi (Subscriber)

```bash
mosquitto_sub -h localhost -t monitor/traffic-flow
```

#### Teste manual (Publisher)

```bash
mosquitto_pub -h IP_RASPBERRY_PI -t monitor/traffic-flow -m "ENTER"
mosquitto_pub -h IP_RASPBERRY_PI -t monitor/traffic-flow -m "EXIT"
```

### Monitoramento

1. **Zabbix Web Interface**: Acesse `http://SEU_IP:8000/zabbix`
2. **Grafana Dashboards**: Acesse `http://SEU_IP:3000`

## 👥 Autores

- **André Luiz Alves de Sousa** - *alas@cesar.school*
- **Henrique Cordeiro Pereira** - *hcp@cesar.school*
- **Leonardo Menezes Soares de Azevedo** - *lmsa@cesar.school*
- **Luiza Omena Suassuna** - *los2@cesar.school*
