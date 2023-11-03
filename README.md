# AutoFeederProject

Projeto de um alimentador automático.

Materiais utilizados:

- ESP8266
- Servo motor MG966R
- RTC module

IDE utilizada:

- VS Code com platformIO

Como trabalhar nesse projeto:

- Com o VSCode instalado, adicionar a extensão PlatformIO
- Clonar esse projeto
- Abrir o platformIO e em 'Home' clicar em 'OPEN PROJECT' e selecionar o projeto clonado
- Alterar a o SSID e senha do WIFI no arquivo WifiCredentials.cpp na pasta '.env'

Para gravar os arquivos html, css, js que ficam na pasta '/data' na placa ESP, pode utilizar o proprio platformIO do VS code:

- Clicar no icone do PlatformIO(barra lateral do VS Code)
- Irá abrir uma seção do lado e em 'Project Tasks', abrir as opçõe do projeto, abrir a opção 'Platform' e clicar em 'Upload FileSystem Image'
  ![Alt text](<VSCode PLtformIO UFI.png>)
