using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BallSpawner : MonoBehaviour
{
    public Transform waypoint0;
    public Transform waypoint1;
    public GameObject ballPrefab;
    public float moveSpeed = 2f;
    public float spawnInterval = 2f;
    public int maxBalls = 10;

    private bool movingRight = true;
    private int ballCount = 0;

    void Start()
    {
        StartCoroutine(MoveSpawner());
        InvokeRepeating(nameof(SpawnBall), 0f, spawnInterval);
    }

    IEnumerator MoveSpawner()
    {
        while (true)
        {
            Transform target = movingRight ? waypoint1 : waypoint0;
            while (Vector2.Distance(transform.position, target.position) > 0.1f)
            {
                transform.position = Vector2.MoveTowards(transform.position, target.position, moveSpeed * Time.deltaTime);
                yield return null;
            }
            movingRight = !movingRight;
            yield return null;
        }
    }

    void SpawnBall()
    {
        if (ballCount >= maxBalls)
        {
            CancelInvoke(nameof(SpawnBall));
            return;
        }

        GameObject newBall = Instantiate(ballPrefab, transform.position, Quaternion.identity);
        Rigidbody2D rb = newBall.GetComponent<Rigidbody2D>();
        if (rb == null)
        {
            rb = newBall.AddComponent<Rigidbody2D>();
        }
        rb.gravityScale = 1;
        ballCount++;
    }
}