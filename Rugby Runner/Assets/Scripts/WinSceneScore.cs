using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;


public class WinSceneScore : MonoBehaviour
{
    [SerializeField] private TMP_Text finalScoreText;
    [SerializeField] private TMP_Text playAgainText;

    void Start()
    {
        int finalScore = PlayerPrefs.GetInt("FinalScore", 0);
        
        if (finalScoreText != null)
        {
            finalScoreText.text = "Final Score: " + finalScore;
        }
        else
        {
            Debug.LogError("Final Score Text is not assigned in the Inspector!");
        }

        if (playAgainText != null)
        {
            playAgainText.text = "PLAY AGAIN";
        }
        else
        {
            Debug.LogError("Play Again Text is not assigned in the Inspector!");
        }
    }
}