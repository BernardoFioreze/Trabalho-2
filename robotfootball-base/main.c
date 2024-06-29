#include <math.h>
#include <chipmunk.h>
#include <SOIL.h>
#include <time.h>
#include <stdlib.h>

// Rotinas para acesso da OpenGL
#include "opengl.h"

// Funções para movimentação de objetos
void moveGoleiroEsq(cpBody* body, void* data);
void moveGoleiroDir(cpBody* body, void* data);
void moveAtacanteEsq1(cpBody* body, void* data);
void moveAtacanteEsq2(cpBody* body, void* data);
void moveAtacanteDir1(cpBody* body, void* data);
void moveAtacanteDir2(cpBody* body, void* data);
void moveDefensorEsq1(cpBody* body, void* data);
void moveDefensorEsq2(cpBody* body, void* data);
void moveDefensorEsq3(cpBody* body, void* data);
void moveDefensorDir1(cpBody* body, void* data);
void moveDefensorDir2(cpBody* body, void* data);
void moveDefensorDir3(cpBody* body, void* data);
void moveBola(cpBody* body, void* data);
int isGoal(cpBody* body);
void robosEsquerdos();
void robosDireitos();
void verificarVelocidadeBola(cpBody* body);
cpBody* acharAtacProximo(cpBody* defBody, cpBody* atacantes[], int numAtac);

// Prototipos
void initCM();
void freeCM();
void restartCM();
cpShape* newLine(cpVect inicio, cpVect fim, cpFloat fric, cpFloat elast);
cpBody* newCircle(cpVect pos, cpFloat radius, cpFloat mass, char* img, bodyMotionFunc func, cpFloat fric, cpFloat elast);

// Score do jogo
int score1 = 0;
int score2 = 0;

// Flag de controle: 1 se o jogo tiver acabado
int gameOver = 0;

// cpVect e' um vetor 2D e cpv() e' uma forma rapida de inicializar ele.
cpVect gravity;

// O ambiente
cpSpace* space;

// Paredes "invisíveis" do ambiente
cpShape* leftWall, *rightWall, *topWall, *bottomWall, *goalwalld, *goalwalle, *goalwalli,  *goalwalldd, *goalwalled, *goalwallid;

// A bola
cpBody* ballBody;

// Um robô
cpBody* robotBody, * goleiroEsq, * goleiroDir, *defEsq1, *defEsq2, *defEsq3, *atacEsq1, *atacEsq2, *defDir1, *defDir2, *defDir3, *atacDir1, *atacDir2;

// Cada passo de simulação é 1/60 seg.
cpFloat timeStep = 1.0/60.0;

// Inicializa o ambiente: é chamada por init() em opengl.c, pois necessita do contexto
// OpenGL para a leitura das imagens
void initCM()
{   
    gravity = cpv(0, 100);

    // Cria o universo
    space = cpSpaceNew();

    // Seta o fator de damping, isto é, de atrito do ar
    cpSpaceSetDamping(space, 0.8);

    // Descomente a linha abaixo se quiser ver o efeito da gravidade!
    //cpSpaceSetGravity(space, gravity);s


    // Adiciona 4 linhas estáticas para formarem as "paredes" do ambiente
    leftWall   = newLine(cpv(44,44), cpv(44,667), 0, 1.0);
    rightWall  = newLine(cpv(979,44), cpv(979,667), 0, 1.0);
    bottomWall = newLine(cpv(44,667), cpv(979,667), 0, 1.0);
    topWall    = newLine(cpv(44,44), cpv(979,44), 0, 1.0);
    //Goleira da esquerda
    goalwalld    = newLine(cpv(26,326), cpv(47,326), 0, 1.0);
    goalwalle    = newLine(cpv(26,388), cpv(47,388), 0, 1.0);
    goalwalli    = newLine(cpv(26,326), cpv(26,388), 0, 1.0);
    //Goleira da direita
    goalwalldd    = newLine(cpv(996,326), cpv(977,326), 0, 1.0);
    goalwalled    = newLine(cpv(996,388), cpv(977,388), 0, 1.0);
    goalwallid    = newLine(cpv(996,326), cpv(996,388), 0, 1.0);

    // Agora criamos a bola...
    // Os parâmetros são:
    //   - posição: cpVect (vetor: x e y)
    //   - raio
    //   - massa
    //   - imagem a ser carregada
    //   - ponteiro para a função de movimentação (chamada a cada passo, pode ser NULL)
    //   - coeficiente de fricção
    //   - coeficiente de elasticidade
    ballBody = newCircle(cpv(512,350), 10, 1, "small_football.png", moveBola, 0.65, 1.1);
    robosEsquerdos();
    robosDireitos();
    // ... e um robô de exemplo
    
    //robosEsquerdos();
    //robosDireitos();
}

void robosEsquerdos(){
    goleiroEsq = newCircle(cpv(50,356), 15, 5, "goleiroInter.png", moveGoleiroEsq, 1, 1);//feito
    defEsq1 = newCircle(cpv(200,178), 15, 6, "defensorInter.png", moveDefensorEsq1, 1, 0.5);//feito
    defEsq2 = newCircle(cpv(200,356), 15, 6, "defensorInter.png", moveDefensorEsq2, 1, 0.5);//feito
    defEsq3 = newCircle(cpv(200,534), 15, 6, "defensorInter.png", moveDefensorEsq3, 1, 0.5);//feito
    atacEsq1 = newCircle(cpv(400,237), 15, 3.5, "atacanteInter.png", moveAtacanteEsq1, 1, 1.5);//feito
    atacEsq2 = newCircle(cpv(400,475), 15, 3.5, "atacanteInter.png", moveAtacanteEsq2, 1, 1.5);//feito
}

void robosDireitos(){
    goleiroDir = newCircle(cpv(974,356), 15, 5, "goleiroGremio.png", moveGoleiroDir, 1, 1);//feito
    defDir1 = newCircle(cpv(824,178), 15, 6, "defensorGremio.png", moveDefensorDir1, 1, 0.5);//feito
    defDir2 = newCircle(cpv(824,356), 15, 6, "defensorGremio.png", moveDefensorDir2, 1, 0.5);//feito
    defDir3 = newCircle(cpv(824,534), 15, 6, "defensorGremio.png", moveDefensorDir3, 1, 0.5);//feito
    atacDir1 = newCircle(cpv(624,237), 15, 3.5, "atacanteGremio.png", moveAtacanteDir1, 1, 1.5);//feito
    atacDir2 = newCircle(cpv(624,475), 15, 3.5, "atacanteGremio.png", moveAtacanteDir2, 1, 1.5);
    
}

// Exemplo de função de movimentação: move o robô em direção à bola
void moveGoleiroEsq(cpBody* body, void* data)
{
    // Veja como obter e limitar a velocidade do robô...
    cpVect vel = cpBodyGetVelocity(body);
    //    printf("vel: %f %f", vel.x,vel.y);

    // Limita o vetor em 50 unidades
    vel = cpvclamp(vel, 50);
    // E seta novamente a velocidade do corpo
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e da bola...
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos  = cpBodyGetPosition(ballBody);
    
    // Calcula um vetor do robô à bola (DELTA = B - R)
    cpVect pos = robotPos;
    pos.x = -robotPos.x;
    pos.y = -robotPos.y;
    cpVect delta = cpvadd(ballPos,pos);

    // Limita o impulso em 20 unidades
    delta = cpvmult(cpvnormalize(delta),50);
    // Finalmente, aplica impulso no robô
    if(robotPos.x <= 191 && robotPos.y >= 178 && robotPos.y <= 537){
        if(ballPos.x <= 191 && ballPos.y >= 178 && ballPos.y <= 537){
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
        }
        else {
            cpVect golPos;
            golPos.x = 90;
            golPos.y = 356;
            cpVect delta = cpvadd(golPos,pos);
            delta = cpvmult(cpvnormalize(delta),50);
            cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
        }
    }
    else {
        cpVect golPos;
         golPos.x = 90;
         golPos.y = 356;
        cpVect delta = cpvadd(golPos,pos);
        delta = cpvmult(cpvnormalize(delta),20);
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
    }
    if(isGoal(ballBody) == 1)
        cpBodySetPosition(body,cpv(50,356));
}

void moveGoleiroDir(cpBody* body, void* data)
{
    // Veja como obter e limitar a velocidade do robô...
    cpVect vel = cpBodyGetVelocity(body);
    //    printf("vel: %f %f", vel.x,vel.y);

    // Limita o vetor em 50 unidades
    vel = cpvclamp(vel, 50);
    // E seta novamente a velocidade do corpo
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e da bola...
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos  = cpBodyGetPosition(ballBody);
    
    // Calcula um vetor do robô à bola (DELTA = B - R)
    cpVect pos = robotPos;
    pos.x = -robotPos.x;
    pos.y = -robotPos.y;
    cpVect delta = cpvadd(ballPos,pos);

    // Limita o impulso em 20 unidades
    delta = cpvmult(cpvnormalize(delta),50);
    // Finalmente, aplica impulso no robô
    if(robotPos.x >= 832 && robotPos.y >= 178 && robotPos.y <= 537){
        if(ballPos.x >= 832 && ballPos.y >= 178 && ballPos.y <= 537){
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
        }
        else {
            cpVect golPos;
            golPos.x = 930;
            golPos.y = 356;
            cpVect delta = cpvadd(golPos,pos);
            delta = cpvmult(cpvnormalize(delta),50);
            cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
        }
    }
    else {
        cpVect golPos;
         golPos.x = 930;
         golPos.y = 356;
        cpVect delta = cpvadd(golPos,pos);
        delta = cpvmult(cpvnormalize(delta),20);
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
    }
    if(isGoal(ballBody) == 1)
        cpBodySetPosition(body,cpv(974,356));
}

void moveAtacanteEsq1(cpBody* body, void* data){
    // Veja como obter e limitar a velocidade do robô...
    cpVect vel = cpBodyGetVelocity(body);
    //    printf("vel: %f %f", vel.x,vel.y);

    // Limita o vetor em 50 unidades
    vel = cpvclamp(vel, 50);
    // E seta novamente a velocidade do corpo
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e da bola...
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos  = cpBodyGetPosition(ballBody);
    
    // Calcula um vetor do robô à bola (DELTA = B - R)
    cpVect pos = robotPos;
    pos.x = -robotPos.x;
    pos.y = -robotPos.y;
    cpVect delta = cpvadd(ballPos,pos);

    // Limita o impulso em 400 unidades
    delta = cpvmult(cpvnormalize(delta),100);
            if(ballPos.x >= 480 && ballPos.y >= 0){
                cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
            }
            else {
                cpVect golPos;
                golPos.x = 400;
                golPos.y = 237;
                cpVect delta = cpvadd(golPos,pos);
                delta = cpvmult(cpvnormalize(delta),100);
                cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
            }
    if(isGoal(ballBody)==1)
        cpBodySetPosition(body,cpv(400,237));
}

void moveAtacanteEsq2(cpBody* body, void* data){
    // Veja como obter e limitar a velocidade do robô...
    cpVect vel = cpBodyGetVelocity(body);
    //    printf("vel: %f %f", vel.x,vel.y);

    // Limita o vetor em 10 unidades
    vel = cpvclamp(vel, 50);
    // E seta novamente a velocidade do corpo
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e da bola...
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos  = cpBodyGetPosition(ballBody);
    
    // Calcula um vetor do robô à bola (DELTA = B - R)
    cpVect pos = robotPos;
    pos.x = -robotPos.x;
    pos.y = -robotPos.y;
    cpVect delta = cpvadd(ballPos,pos);

    // Limita o impulso em 400 unidades
    delta = cpvmult(cpvnormalize(delta),100);
            if(ballPos.x >= 480 && ballPos.y >= 0){
                cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
            }
            else {
                cpVect golPos;
                golPos.x = 400;
                golPos.y = 475;
                cpVect delta = cpvadd(golPos,pos);
                delta = cpvmult(cpvnormalize(delta),100);
                cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
            }
    if(isGoal(ballBody)==1)
        cpBodySetPosition(body,cpv(400,475));
}

void moveAtacanteDir1(cpBody* body, void* data){
    // Veja como obter e limitar a velocidade do robô...
    cpVect vel = cpBodyGetVelocity(body);
    //    printf("vel: %f %f", vel.x,vel.y);

    // Limita o vetor em 10 unidades
    vel = cpvclamp(vel, 50);
    // E seta novamente a velocidade do corpo
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e da bola...
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos  = cpBodyGetPosition(ballBody);
    
    // Calcula um vetor do robô à bola (DELTA = B - R)
    cpVect pos = robotPos;
    pos.x = -robotPos.x;
    pos.y = -robotPos.y;
    cpVect delta = cpvadd(ballPos,pos);

    // Limita o impulso em 400 unidades
    delta = cpvmult(cpvnormalize(delta),100);
            if(ballPos.x <= 550 && ballPos.y >= 0){
            cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
        }
            else{
                cpVect golPos;
                golPos.x = 624;
                golPos.y = 237;
                cpVect delta = cpvadd(golPos,pos);
                delta = cpvmult(cpvnormalize(delta),100);
                cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
            }
    if(isGoal(ballBody)==1)
        cpBodySetPosition(body,cpv(624,237));      
}

void moveAtacanteDir2(cpBody* body, void* data){
    // Veja como obter e limitar a velocidade do robô...
    cpVect vel = cpBodyGetVelocity(body);
    //    printf("vel: %f %f", vel.x,vel.y);

    // Limita o vetor em 50 unidades
    vel = cpvclamp(vel, 50);
    // E seta novamente a velocidade do corpo
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e da bola...
    cpVect robotPos = cpBodyGetPosition(body);
    cpVect ballPos  = cpBodyGetPosition(ballBody);
    
    // Calcula um vetor do robô à bola (DELTA = B - R)
    cpVect pos = robotPos;
    pos.x = -robotPos.x;
    pos.y = -robotPos.y;
    cpVect delta = cpvadd(ballPos,pos);

    // Limita o impulso em 400 unidades
    delta = cpvmult(cpvnormalize(delta),100);
            if(ballPos.x <= 550 && ballPos.y >= 0){
            cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
        }
            else{
                cpVect golPos;
                golPos.x = 624;
                golPos.y = 475;
                cpVect delta = cpvadd(golPos,pos);
                delta = cpvmult(cpvnormalize(delta),100);
                cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
            }
    if(isGoal(ballBody)==1)
        cpBodySetPosition(body,cpv(624,475));        
}

void moveDefensorEsq1(cpBody* body, void* data){
    // Obter e limitar a velocidade do robô...
    cpVect vel = cpBodyGetVelocity(body);
    // printf("vel: %f %f", vel.x,vel.y);

    // Limita o vetor em 50 unidades
    vel = cpvclamp(vel, 50);
    // E seta novamente a velocidade do corpo
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e do atacante...
    cpVect robotPos = cpBodyGetPosition(body);
    
    cpVect atacPos = cpBodyGetPosition(atacDir1);

    // Verificar se o defensor está no seu lado do campo
    if (robotPos.x <= 512 && robotPos.y >= 0) {
        // Verificar se o atacante está no lado do campo do defensor
        if (atacPos.x <= 512 && atacPos.y >= 0) {
            // Calcular o vetor do defensor para o atacante (DELTA = A - D)
            cpVect delta = cpvsub(atacPos, robotPos);
            delta = cpvmult(cpvnormalize(delta), 100);
    
            // Aplicar impulso na direção do atacante
            cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
        } else {
            // Se o atacante não estiver no lado do campo do defensor, mover o defensor para uma posição inicial
            cpVect newPos;
            newPos.x = 200;
            newPos.y = 178;
            cpVect delta = cpvsub(newPos, robotPos);
            delta = cpvmult(cpvnormalize(delta), 100);

            // Aplicar impulso na direção da posição inicial
            cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
        }
    } else {
        // Se o defensor não estiver no seu lado do campo, mover o defensor para uma posição inicial
        cpVect newPos;
        newPos.x = 200;
        newPos.y = 178;
        cpVect delta = cpvsub(newPos, robotPos);
        delta = cpvmult(cpvnormalize(delta), 100);

        // Aplicar impulso na direção da posição inicial
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
    }
    if(isGoal(ballBody)==1)
        cpBodySetPosition(body,cpv(200,178));

}

void moveDefensorEsq2(cpBody* body, void* data){
    // Obter e limitar a velocidade do robô...
    cpVect vel = cpBodyGetVelocity(body);
    // printf("vel: %f %f", vel.x,vel.y);

    // Limita o vetor em 50 unidades
    vel = cpvclamp(vel, 50);
    // E seta novamente a velocidade do corpo
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e do atacante...
    cpVect robotPos = cpBodyGetPosition(body);
    cpBody * atacantes[] = {atacDir1, atacDir2};
    int numAtac = 2;

    cpBody * atacProximo = acharAtacProximo(body, atacantes, numAtac);
    cpVect atacPos = cpBodyGetPosition(atacProximo);

    // Verificar se o defensor está no seu lado do campo
    if (robotPos.x <= 512 && robotPos.y >= 0) {
        // Verificar se o atacante está no lado do campo do defensor
        if (atacPos.x <= 512 && atacPos.y >= 0) {
            // Calcular o vetor do defensor para o atacante (DELTA = A - D)
            cpVect delta = cpvsub(atacPos, robotPos);
            delta = cpvmult(cpvnormalize(delta), 100);
    
            // Aplicar impulso na direção do atacante
            cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
        } else {
            // Se o atacante não estiver no lado do campo do defensor, mover o defensor para uma posição inicial
            cpVect newPos;
            newPos.x = 200;
            newPos.y = 356;
            cpVect delta = cpvsub(newPos, robotPos);
            delta = cpvmult(cpvnormalize(delta), 100);

            // Aplicar impulso na direção da posição inicial
            cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
        }
    } else {
        // Se o defensor não estiver no seu lado do campo, mover o defensor para uma posição inicial
        cpVect newPos;
        newPos.x = 200;
        newPos.y = 356;
        cpVect delta = cpvsub(newPos, robotPos);
        delta = cpvmult(cpvnormalize(delta), 100);

        // Aplicar impulso na direção da posição inicial
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
    }
    if(isGoal(ballBody)==1)
        cpBodySetPosition(body,cpv(200,356));

}

void moveDefensorEsq3(cpBody* body, void* data){
    // Obter e limitar a velocidade do robô...
    cpVect vel = cpBodyGetVelocity(body);
    // printf("vel: %f %f", vel.x,vel.y);

    // Limita o vetor em 50 unidades
    vel = cpvclamp(vel, 50);
    // E seta novamente a velocidade do corpo
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e do atacante...
    cpVect robotPos = cpBodyGetPosition(body);
    
    cpVect atacPos = cpBodyGetPosition(atacDir2);

    // Verificar se o defensor está no seu lado do campo
    if (robotPos.x <= 512 && robotPos.y >= 0) {
        // Verificar se o atacante está no lado do campo do defensor
        if (atacPos.x <= 512 && atacPos.y >= 0) {
            // Calcular o vetor do defensor para o atacante (DELTA = A - D)
            cpVect delta = cpvsub(atacPos, robotPos);
            delta = cpvmult(cpvnormalize(delta), 100);
    
            // Aplicar impulso na direção do atacante
            cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
        } else {
            // Se o atacante não estiver no lado do campo do defensor, mover o defensor para uma posição inicial
            cpVect newPos;
            newPos.x = 200;
            newPos.y = 534;
            cpVect delta = cpvsub(newPos, robotPos);
            delta = cpvmult(cpvnormalize(delta), 100);

            // Aplicar impulso na direção da posição inicial
            cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
        }
    } else {
        // Se o defensor não estiver no seu lado do campo, mover o defensor para uma posição inicial
        cpVect newPos;
        newPos.x = 200;
        newPos.y = 534;
        cpVect delta = cpvsub(newPos, robotPos);
        delta = cpvmult(cpvnormalize(delta), 100);

        // Aplicar impulso na direção da posição inicial
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
    }
    if(isGoal(ballBody)==1)
        cpBodySetPosition(body,cpv(200,534));

}

void moveDefensorDir1(cpBody* body, void* data){
    // Obter e limitar a velocidade do robô...
    cpVect vel = cpBodyGetVelocity(body);
    // printf("vel: %f %f", vel.x,vel.y);

    // Limita o vetor em 50 unidades
    vel = cpvclamp(vel, 50);
    // E seta novamente a velocidade do corpo
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e do atacante...
    cpVect robotPos = cpBodyGetPosition(body);
    
    cpVect atacPos = cpBodyGetPosition(atacEsq1);

    // Verificar se o defensor está no seu lado do campo
    if (robotPos.x >= 512 && robotPos.y >= 0) {
        // Verificar se o atacante está no lado do campo do defensor
        if (atacPos.x >= 512 && atacPos.y >= 0) {
            // Calcular o vetor do defensor para o atacante (DELTA = A - D)
            cpVect delta = cpvsub(atacPos, robotPos);
            delta = cpvmult(cpvnormalize(delta), 100);
    
            // Aplicar impulso na direção do atacante
            cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
        } else {
            // Se o atacante não estiver no lado do campo do defensor, mover o defensor para uma posição inicial
            cpVect newPos;
            newPos.x = 824;
            newPos.y = 178;
            cpVect delta = cpvsub(newPos, robotPos);
            delta = cpvmult(cpvnormalize(delta), 100);

            // Aplicar impulso na direção da posição inicial
            cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
        }
    } else {
        // Se o defensor não estiver no seu lado do campo, mover o defensor para uma posição inicial
        cpVect newPos;
        newPos.x = 824;
        newPos.y = 178;
        cpVect delta = cpvsub(newPos, robotPos);
        delta = cpvmult(cpvnormalize(delta), 100);

        // Aplicar impulso na direção da posição inicial
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
    }
    if(isGoal(ballBody)==1)
        cpBodySetPosition(body,cpv(824,178));
}

void moveDefensorDir2(cpBody* body, void* data){
    // Obter e limitar a velocidade do robô...
    cpVect vel = cpBodyGetVelocity(body);
    // printf("vel: %f %f", vel.x,vel.y);

    // Limita o vetor em 10 unidades
    vel = cpvclamp(vel, 50);
    // E seta novamente a velocidade do corpo
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e do atacante...
    cpVect robotPos = cpBodyGetPosition(body);
    cpBody * atacantes[] = {atacEsq1, atacEsq2};
    int numAtac = 2;

    cpBody * atacProximo = acharAtacProximo(body, atacantes, numAtac);
    cpVect atacPos = cpBodyGetPosition(atacProximo);

    // Verificar se o defensor está no seu lado do campo
    if (robotPos.x >= 512 && robotPos.y >= 0) {
        // Verificar se o atacante está no lado do campo do defensor
        if (atacPos.x >= 512 && atacPos.y >= 0) {
            // Calcular o vetor do defensor para o atacante (DELTA = A - D)
            cpVect delta = cpvsub(atacPos, robotPos);
            delta = cpvmult(cpvnormalize(delta), 100);
    
            // Aplicar impulso na direção do atacante
            cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
        } else {
            // Se o atacante não estiver no lado do campo do defensor, mover o defensor para uma posição inicial
            cpVect newPos;
            newPos.x = 824;
            newPos.y = 356;
            cpVect delta = cpvsub(newPos, robotPos);
            delta = cpvmult(cpvnormalize(delta), 100);

            // Aplicar impulso na direção da posição inicial
            cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
        }
    } else {
        // Se o defensor não estiver no seu lado do campo, mover o defensor para uma posição inicial
        cpVect newPos;
        newPos.x = 824;
        newPos.y = 356;
        cpVect delta = cpvsub(newPos, robotPos);
        delta = cpvmult(cpvnormalize(delta), 100);

        // Aplicar impulso na direção da posição inicial
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
    }
    if(isGoal(ballBody)==1)
        cpBodySetPosition(body,cpv(824,356));
}

void moveDefensorDir3(cpBody* body, void* data){
    // Obter e limitar a velocidade do robô...
    cpVect vel = cpBodyGetVelocity(body);
    // printf("vel: %f %f", vel.x,vel.y);

    // Limita o vetor em 10 unidades
    vel = cpvclamp(vel, 50);
    // E seta novamente a velocidade do corpo
    cpBodySetVelocity(body, vel);

    // Obtém a posição do robô e do atacante...
    cpVect robotPos = cpBodyGetPosition(body);

    cpVect atacPos = cpBodyGetPosition(atacEsq2);
    
    // Verificar se o defensor está no seu lado do campo
    if (robotPos.x >= 512 && robotPos.y >= 0) {
        // Verificar se o atacante está no lado do campo do defensor
        if (atacPos.x >= 512 && atacPos.y >= 0) {
            // Calcular o vetor do defensor para o atacante (DELTA = A - D)
            cpVect delta = cpvsub(atacPos, robotPos);
            delta = cpvmult(cpvnormalize(delta), 100);
    
            // Aplicar impulso na direção do atacante
            cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
        } else {
            // Se o atacante não estiver no lado do campo do defensor, mover o defensor para uma posição inicial
            cpVect newPos;
            newPos.x = 824;
            newPos.y = 534;
            cpVect delta = cpvsub(newPos, robotPos);
            delta = cpvmult(cpvnormalize(delta), 100);

            // Aplicar impulso na direção da posição inicial
            cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
        }
    } else {
        // Se o defensor não estiver no seu lado do campo, mover o defensor para uma posição inicial
        cpVect newPos;
        newPos.x = 824;
        newPos.y = 534;
        cpVect delta = cpvsub(newPos, robotPos);
        delta = cpvmult(cpvnormalize(delta), 100);

        // Aplicar impulso na direção da posição inicial
        cpBodyApplyImpulseAtWorldPoint(body, delta, robotPos);
    }
    if(isGoal(ballBody)==1)
        cpBodySetPosition(body,cpv(824,534));

}
int isGoal(cpBody* body){
    if(score1 == 2){
        score1++;
        freeCM();
    }
    if(score2 == 2){
        score2++;
        freeCM();
    }
    cpVect ballPos = cpBodyGetPosition(body);
    if(ballPos.x <= 995 && ballPos.x >= 970 && ballPos.y >= 327 && ballPos.y <= 386){
        score1++;
        return 1;
    }
    else if(ballPos.x <= 53 && ballPos.x >= 27 && ballPos.y >= 327 && ballPos.y <= 386){
        score2++;
        return 1;
    }
    return 0;


}

// Exemplo de função de movimentacao: move a bola
void moveBola(cpBody* body, void* data)
{
    cpVect impulso = cpv(0, 0);
    cpBodyApplyImpulseAtWorldPoint(body, impulso, cpBodyGetPosition(body));
    cpVect ballPos  = cpBodyGetPosition(ballBody);

    // Se for gol

    //Bola fora do campo
    if(ballPos.x < 44 || ballPos.x > 979 || ballPos.y < 44 || ballPos.y > 667){
        cpVect ballPos  = cpBodyGetPosition(ballBody);
        cpVect newPos;
        newPos.x = 512;
        newPos.y = 350;
        cpBodySetPosition(ballBody, newPos);
        cpVect ballSpeed = cpBodyGetVelocity(ballBody);
        cpVect newSpeed;
        newSpeed.x = 0;
        newSpeed.y = 0;
        cpBodySetVelocity(ballBody, newSpeed);
    }

    cpVect ballSpeed = cpBodyGetVelocity(ballBody);
    //Teste para ver se a bola para de sair do campo
    if(abs(ballSpeed.x) > 100 && abs(ballSpeed.y) > 100){
        cpVect ballSpeed = cpBodyGetVelocity(ballBody);
        cpVect newSpeed;
        newSpeed.x = 100;
        newSpeed.y = 100;
        cpBodySetVelocity(ballBody, newSpeed);
    }
    if(isGoal(body)==1)
        cpBodySetPosition(body,cpv(512,350));

    
}


void verificarVelocidadeBola(cpBody* body) {
    cpVect vel = cpBodyGetVelocity(body);
    cpFloat speed = cpvlength(vel);
    printf("Velocidade da bola: %f\n", speed);
}

cpBody* acharAtacProximo(cpBody* defBody, cpBody* atacantes[], int numAtac) {
    cpVect defPos = cpBodyGetPosition(defBody);
    cpBody* atacProx= NULL;
    cpFloat distanciaMin= INFINITY;

    for (int i = 0; i < numAtac;i++) {
        cpVect posAtac= cpBodyGetPosition(atacantes[i]);
        cpFloat distancia = cpvlength(cpvsub(posAtac, defPos));
        if (distancia < distanciaMin) {
            distanciaMin = distancia;
            atacProx = atacantes[i];
        }
    }

    return atacProx;
}

// Libera memória ocupada por cada corpo, forma e ambiente
// Acrescente mais linhas caso necessário
void freeCM()
{   
    printf("Cleaning up!\n");
    UserData* ud = cpBodyGetUserData(ballBody);
    cpShapeFree(ud->shape);
    cpBodyFree(ballBody);

    ud = cpBodyGetUserData(robotBody);
    cpShapeFree(ud->shape);
    cpBodyFree(robotBody);

    cpShapeFree(leftWall);
    cpShapeFree(rightWall);
    cpShapeFree(bottomWall);
    cpShapeFree(topWall);

    cpSpaceFree(space);
}

// Função chamada para reiniciar a simulação
void restartCM()
{
    // Escreva o código para reposicionar os jogadores, ressetar o score, etc.
    if(gameOver == 1){
        cpVect ballPos  = cpBodyGetPosition(ballBody);
        cpVect newPos;
        newPos.x = 512;
        newPos.y = 350;
        cpBodySetPosition(ballBody, newPos);
        cpVect ballSpeed = cpBodyGetVelocity(ballBody);
        cpVect newSpeed;
        newSpeed.x = 0;
        newSpeed.y = 0;
        cpBodySetVelocity(ballBody, newSpeed);
        robosDireitos();
        robosEsquerdos();
    }
    // Não esqueça de ressetar a variável gameOver!
    gameOver = 0;
}

// ************************************************************
//
// A PARTIR DESTE PONTO, O PROGRAMA NÃO DEVE SER ALTERADO
//
// A NÃO SER QUE VOCÊ SAIBA ***EXATAMENTE*** O QUE ESTÁ FAZENDO
//
// ************************************************************

int main(int argc, char** argv)
{
    // Inicialização da janela gráfica
    init(argc,argv);

    // Não retorna... a partir daqui, interação via teclado e mouse apenas, na janela gráfica
    glutMainLoop();
    return 0;
}

// Cria e adiciona uma nova linha estática (segmento) ao ambiente
cpShape* newLine(cpVect inicio, cpVect fim, cpFloat fric, cpFloat elast)
{
   cpShape* aux = cpSegmentShapeNew(cpSpaceGetStaticBody(space), inicio, fim, 0);
   cpShapeSetFriction(aux, fric);
   cpShapeSetElasticity(aux, elast);
   cpSpaceAddShape(space, aux);
   return aux;
}

// Cria e adiciona um novo corpo dinâmico, com formato circular
cpBody* newCircle(cpVect pos, cpFloat radius, cpFloat mass, char* img, bodyMotionFunc func, cpFloat fric, cpFloat elast)
{
    // Primeiro criamos um cpBody para armazenar as propriedades fisicas do objeto
    // Estas incluem: massa, posicao, velocidade, angulo, etc do objeto
    // A seguir, adicionamos formas de colisao ao cpBody para informar o seu formato e tamanho

    // O momento de inercia e' como a massa, mas para rotacao
    // Use as funcoes cpMomentFor*() para calcular a aproximacao dele
    cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);

    // As funcoes cpSpaceAdd*() retornam o que voce esta' adicionando
    // E' conveniente criar e adicionar um objeto na mesma linha
    cpBody* newBody = cpSpaceAddBody(space, cpBodyNew(mass, moment));

    // Por fim, ajustamos a posicao inicial do objeto
    cpBodySetPosition(newBody, pos);

    // Agora criamos a forma de colisao do objeto
    // Voce pode criar multiplas formas de colisao, que apontam ao mesmo objeto (mas nao e' necessario para o trabalho)
    // Todas serao conectadas a ele, e se moverao juntamente com ele
    cpShape* newShape = cpSpaceAddShape(space, cpCircleShapeNew(newBody, radius, cpvzero));
    cpShapeSetFriction(newShape, fric);
    cpShapeSetElasticity(newShape, elast);

    UserData* newUserData = malloc(sizeof(UserData));
    newUserData->tex = loadImage(img);
    newUserData->radius = radius;
    newUserData->shape= newShape;
    newUserData->func = func;
    cpBodySetUserData(newBody, newUserData);
    printf("newCircle: loaded img %s\n", img);
    return newBody;
}
