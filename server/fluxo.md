
## Fluxo de escuta

- recebe nome de usuário do cliente (não printa mais as boas-vindas)
- recebe opção 1 ou 2
    - 1: criar nova sala
    - 2: mostra salas existentes
- escuta/encaminha mensagens dos clientes de uma determinada sala

### 

- criar nova sala:
    - recebe string com nome da sala
- mostra salas existentes:
    - se já existem salas, recebe input do usuário...
        - 1: [nome da sala 1]
        - n: [nome da sala 2]
        - 0: retorna ao menu anterior
    ... e aguarda input com número da sala, que deverá ser implementado no lado do cliente
    - se não, exibe mensagem de que não existem salas ainda