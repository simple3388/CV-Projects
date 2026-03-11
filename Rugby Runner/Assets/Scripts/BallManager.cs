using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class BallManager : MonoBehaviour
{
    public static BallManager Instance;
    private int ballCount = 0;
    [SerializeField] int maxBalls = 10;
    [SerializeField] float gameOverDelay = 0.5f;

    private void Awake()
    {
        if (Instance == null)
        {
            Instance = this;
        }
    }

    public void BallCollectedOrDestroyed()
    {
        ballCount++;
        Debug.Log("Balls Count: " + ballCount);

        if (ballCount >= maxBalls)
        {
            StartCoroutine(LoadGameOver());
        }
    }

    private IEnumerator LoadGameOver()
    {
         yield return new WaitForSeconds(gameOverDelay);
        SceneManager.LoadScene("WinScene");
    }
}
