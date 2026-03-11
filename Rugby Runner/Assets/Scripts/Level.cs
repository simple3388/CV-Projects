using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;


public class Level : MonoBehaviour
{
    [SerializeField] float delayInSeconds = 1f;

    IEnumerator WaitAndLoad() {
        yield return new WaitForSeconds(delayInSeconds);
        SceneManager.LoadScene("GameOver");
    }

    public void LoadStartMenu() {
        SceneManager.LoadScene(0);
    }

    public void LoadGame()
    {
        if (ScoreManager.Instance != null)
        {
            ScoreManager.Instance.ResetScore();
        }
        SceneManager.LoadScene("RugbyRunner");
    }

    public void LoadGameOver() {
        StartCoroutine(WaitAndLoad());
    }

    public void LoadWinScene()
    {
        SceneManager.LoadScene("WinScene");
    }

    public void Quit() {
        Application.Quit();
        
        #if UNITY_EDITOR
        UnityEditor.EditorApplication.isPlaying = false;
        #endif
    }
}
