#pragma once
#include <raylib.h>
#include <vector>
#include "game_object.h"
#include "log_manager.h"

namespace Engine {

    class Enemy : public GameObject {
        private:
            float speed;
            Color color;
            Vector3 targetPosition;
            bool reachedTarget = false;
        public:
            Enemy();

            void Spawn(Vector3 spawnPosition, Vector3 targetPosition, Quaternion rotation, Vector3 scale, float speed, Color color, LogManager& eventLog);

            void Draw() override;
            void Update(float deltaTime) override;

            void MoveTowardsTarget(Vector3 targetPosition, float deltaTime);

            bool HasReachedTarget() const;
            void SetTargetPosition(Vector3 targetPosition);
    };

    class EnemyManager {
        private:
            Vector3 centerPosition;
            Vector3 targetPosition;
            std::vector<Enemy> pool;
            size_t activeEnemies;
            float spawnPeriod;
            float spawnTimer;
            size_t maxEnemies;
            float spawnRadius;
            float minSpeed;
            float maxSpeed;
        public:
            EnemyManager(Vector3 centerPosition, Vector3 targetPosition, float spawnPeriod, int maxEnemies, float spawnRadius, float minSpeed, float maxSpeed);

            Color calculateColor();
            float calculateSpeed();
            Vector3 calculateSpawnPosition();

            void SpawnEnemy(LogManager& eventLog);
            void Update(float deltaTime, LogManager& eventLog);
            void Draw();
    };

}
