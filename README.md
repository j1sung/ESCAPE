### 🎯Project Goal
C++로 직접 구현한 콘솔 기반 게임 엔진에서 A* Pathfinding 알고리즘을 시각화하고, 이를 게임 플레이에 적용하는 실습 프로젝트입니다.

### 💡Game Overview
주어진 시간 안에 3개의 맵을 모두 클리어해야 하는 타임어택 방식의 전략 퍼즐 길찾기 게임입니다.
플레이어는 벽을 배치하거나 제거하여 경로를 조정하고 목적지까지 안전하게 도달해야 합니다.

**Game Conditions**

- 🏆 **Victory**: 제한 시간 내에 3개의 스테이지를 죽지 않고 연속 클리어
- 💀 **Defeat**: 위험 지역(!)을 지나가거나 제한 시간이 초과될 경우

### 🎮 Controls
|Key / Input|Action|
|:---:|:---:|
|Mouse Left Click|벽 배치 / 벽 제거|
|Space|목적지 탐색 시작 (Pathfinding 실행)|
