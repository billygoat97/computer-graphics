# computer-graphics 1차 과제

Sun, Earth, Moon 3가지를 기반으로 Sun을 중심으로 Earth가 그 주위를 맴돌며, moon이 rotate하는 earth를 중심으로 다시 rotate하는 과제이다
sun은 고정되어있는 것에 반해, earth는 자전을 하면서 동시에 공전 처리해 주어야 하기 떄문에 rotate function을 잘 이용해야 하며,
moon은 rotating하는 earth를 원점으로 회전을 하기 때문에 rotate하는 점을 중심이 아닌 지구의 중심으로 하여 처리를 해야 하며

sun은 빛의 영향을 받지 않으며, earth는 sun의 빛을 받는 부분과 아닌 부분을 shading을 통해서 처리를 해주며, moon은 earth의 shading에 영향을 받지 않는 조건으로 진행하였다.

earth texture을 입히기 위해서 상하좌우를 처리했음.
