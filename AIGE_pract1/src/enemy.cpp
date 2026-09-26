#include "enemy.h"
#include <raymath.h>
#include "utils.h"

namespace Engine {

    Enemy::Enemy() : speed(0.0f), color(WHITE), targetPosition({ 0.0f, 0.0f, 0.0f }) {

    }

    void Enemy::Spawn(Vector3 spawnPosition, Vector3 targetPosition, Quaternion rotation, Vector3 scale, float speed, Color color, LogManager& eventLog) {
        eventLog.AddLog("Enemy spawned");
        transform.position = spawnPosition;
        transform.rotation = rotation;
        transform.scale = scale;
        this->targetPosition = targetPosition;
        this->speed = speed;
        this->color = color;
        this->reachedTarget = false;
    }

    void Enemy::Draw() {
        DrawCube(transform.position, transform.scale.x, transform.scale.y, transform.scale.z, color);
    }

    void Enemy::Update(float deltaTime) {
        MoveTowardsTarget(targetPosition, deltaTime);
    }

    void Enemy::MoveTowardsTarget(Vector3 targetPosition, float deltaTime) {
        Vector3 direction = targetPosition - transform.position;
        float distance = Vector3Length(direction);
        if (distance > 0.5f) {
            Vector3 normalizedDirection = Vector3Normalize(direction);
            transform.position += normalizedDirection * speed * deltaTime;
        } else {
            reachedTarget = true;
        }
    }

    bool Enemy::HasReachedTarget() const {
        return reachedTarget;
    }

    void Enemy::SetTargetPosition(Vector3 targetPosition) {
        this->targetPosition = targetPosition;
    }

    EnemyManager::EnemyManager(Vector3 centerPosition, Vector3 targetPosition, float spawnPeriod, int maxEnemies, float spawnRadius, float minSpeed, float maxSpeed)
        : centerPosition(centerPosition), targetPosition(targetPosition), spawnPeriod(spawnPeriod), maxEnemies(maxEnemies), spawnRadius(spawnRadius), minSpeed(minSpeed), maxSpeed(maxSpeed) {
        pool.resize(maxEnemies);
        activeEnemies = 0;
        spawnTimer = 0.0f;
    }

    Color EnemyManager::calculateColor() {
        return { (unsigned char)GetRandomValue(0, 255), (unsigned char)GetRandomValue(0, 255), (unsigned char)GetRandomValue(0, 255), 255 };
    }

    float EnemyManager::calculateSpeed() {
        return GetRandomFloat(minSpeed, maxSpeed);
    }

    Vector3 EnemyManager::calculateSpawnPosition() {
        float angle = GetRandomFloat(0, 360) * DEG2RAD;
        float distance = spawnRadius;
        return { centerPosition.x + distance * cos(angle), 1.0, centerPosition.z + distance * sin(angle) };
    }

    void EnemyManager::SpawnEnemy(LogManager& eventLog) {
        if (activeEnemies < maxEnemies && spawnTimer >= spawnPeriod) {
            pool[activeEnemies].Spawn(calculateSpawnPosition(), targetPosition, { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, calculateSpeed(), calculateColor(), eventLog);
            activeEnemies++;
            spawnTimer = 0.0f;
        }
    }

    void EnemyManager::Update(float deltaTime, LogManager& eventLog) {
        spawnTimer += deltaTime;
        SpawnEnemy(eventLog);
        for (size_t i = 0; i < activeEnemies; i++) {
            pool[i].SetTargetPosition(targetPosition);
            pool[i].Update(deltaTime);

            if (pool[i].HasReachedTarget()) {
                eventLog.AddLog("Enemy reached target");
                std::swap(pool[i], pool[activeEnemies - 1]);
                activeEnemies--;
                i--;
            }
        }
    }

    void EnemyManager::Draw() {
        for (size_t i = 0; i < activeEnemies; i++) {
            pool[i].Draw();
        }
    }

}
