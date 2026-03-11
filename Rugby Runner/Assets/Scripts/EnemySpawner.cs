using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class EnemySpawner : MonoBehaviour
{
    public Transform[] spawnPoints;
    public GameObject[] enemyPrefabs;
    public float spawnInterval = 2.5f;
    public float[] spawnDelays;

    void Start()
    {
        for (int i = 0; i < spawnPoints.Length; i++)
        {
            float delay = (i < spawnDelays.Length) ? spawnDelays[i] : 0f;
            StartCoroutine(SpawnEnemies(spawnPoints[i], delay));
        }
    }

    IEnumerator SpawnEnemies(Transform spawnPoint, float initialDelay)
    {
        yield return new WaitForSeconds(initialDelay);

        while (true)
        {
            SpawnRandomEnemy(spawnPoint);
            yield return new WaitForSeconds(spawnInterval);
        }
    }

    void SpawnRandomEnemy(Transform spawnPoint)
    {
        if (enemyPrefabs.Length == 0) return;

        int randomIndex = Random.Range(0, enemyPrefabs.Length);
        Instantiate(enemyPrefabs[randomIndex], spawnPoint.position, Quaternion.identity);
    }
}