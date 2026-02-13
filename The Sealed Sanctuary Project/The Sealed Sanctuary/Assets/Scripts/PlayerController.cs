using UnityEngine;

public class PlayerController : MonoBehaviour
{
    private GridMovement gridMovement;
    private MoveCounterUI moveCounter;

    private void Awake()
    {
        gridMovement = GetComponent<GridMovement>();
        moveCounter = GameObject.FindObjectOfType<MoveCounterUI>();
    }

    private void OnEnable()
    {
        // Optionally, re-fetch the MoveCounterUI in case of scene reloads
        moveCounter = GameObject.FindObjectOfType<MoveCounterUI>();
    }

    private void Update()
    {
        if (gridMovement.IsMoving)
            return;

        Vector2Int direction = GetInputDirection();

        if (direction != Vector2Int.zero)
        {
            gridMovement.TryMove(direction);
            HandlePlayerMove();
        }
    }

    private Vector2Int GetInputDirection()
    {
        if (Input.GetKeyDown(KeyCode.W) || Input.GetKeyDown(KeyCode.UpArrow))
            return Vector2Int.up;

        if (Input.GetKeyDown(KeyCode.S) || Input.GetKeyDown(KeyCode.DownArrow))
            return Vector2Int.down;

        if (Input.GetKeyDown(KeyCode.A) || Input.GetKeyDown(KeyCode.LeftArrow))
            return Vector2Int.left;

        if (Input.GetKeyDown(KeyCode.D) || Input.GetKeyDown(KeyCode.RightArrow))
            return Vector2Int.right;

        return Vector2Int.zero;
    }

    private void HandlePlayerMove()
    {
        if (moveCounter != null)
        {
            moveCounter.IncrementMoveCount();
            // Hard mode: check move limit (always use current value)
            if (GameData.HardModeEnabled)
            {
                int moveLimit = 0;
                if (GameData.CurrentLevelIndex == 1)
                    moveLimit = GameData.Level1MoveLimit;
                else if (GameData.CurrentLevelIndex == 2)
                    moveLimit = GameData.Level2MoveLimit;
                if (moveLimit > 0 && moveCounter.GetMoveCount() > moveLimit)
                {
                    LevelManager.Instance.GoToLoseScene();
                }
            }
        }
    }
}
