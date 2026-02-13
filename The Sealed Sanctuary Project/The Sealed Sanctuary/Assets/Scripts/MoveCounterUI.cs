using UnityEngine;
using UnityEngine.UI;

public class MoveCounterUI : MonoBehaviour
{
    public Text moveCounterText;
    private int moveCount = 0;

    public void IncrementMoveCount()
    {
        moveCount++;
        UpdateUI();
    }

    public void ResetMoveCount()
    {
        moveCount = 0;
        UpdateUI();
    }

    public int GetMoveCount()
    {
        return moveCount;
    }

    private void UpdateUI()
    {
        if (moveCounterText != null)
            moveCounterText.text = $"Moves: {moveCount}";
    }
}
