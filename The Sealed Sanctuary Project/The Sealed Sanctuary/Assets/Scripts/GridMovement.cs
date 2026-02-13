using System.Collections;
using UnityEngine;
#if UNITY_EDITOR
using UnityEditor;
#endif

public class GridMovement : MonoBehaviour
{
    [SerializeField] private float moveSpeed = 5f;
    [SerializeField] private LayerMask blockingLayer;

    public bool IsMoving { get; private set; }

    private Vector3 targetPosition;

    private void Start()
    {
        targetPosition = transform.position;
    }

    public void TryMove(Vector2Int direction)
    {
        if (IsMoving)
            return;

        Vector3 moveVector = new Vector3(direction.x, direction.y, 0f);
        Vector3 destination = transform.position + moveVector;

        // Use grid origin and size registered in GameData to allow reaching edge tiles exactly
        int destCol = Mathf.RoundToInt(destination.x - GameData.GridStartX);
        int destRow = Mathf.RoundToInt(destination.y - GameData.GridStartY);

        // Check bounds
        if (destCol < 0 || destCol >= GameData.GridCols || destRow < 0 || destRow >= GameData.GridRows)
            return;

        // Check what is occupying the destination. Use OverlapBoxAll and pick a collider whose transform.position is centered on the destination (within epsilon). This
        // avoids detecting neighboring colliders that slightly overlap due to collider size.
        Collider2D hit = null;
        Collider2D[] hits = Physics2D.OverlapBoxAll(destination, Vector2.one * 0.9f, 0f, blockingLayer);
        const float centerEpsilon = 0.15f;
        foreach (var h in hits)
        {
            if (Mathf.Abs(h.transform.position.x - destination.x) < centerEpsilon && Mathf.Abs(h.transform.position.y - destination.y) < centerEpsilon)
            {
                hit = h;
                break;
            }
        }

        if (hit != null)
        {
            Pushable pushable = hit.GetComponent<Pushable>();
            if (pushable != null)
            {
                // Attempt to push the object. If it can't be pushed, block movement.
                if (!pushable.TryPush(direction, blockingLayer))
                    return;
            }
            else
            {
                return; // blocked by a non-pushable object
            }
        }

        targetPosition = destination;
        StartCoroutine(Move());
    }

    private IEnumerator Move()
    {
        IsMoving = true;

        while (Vector3.Distance(transform.position, targetPosition) > 0.01f)
        {
            transform.position = Vector3.MoveTowards(
                transform.position,
                targetPosition,
                moveSpeed * Time.deltaTime
            );
            yield return null;
        }

        transform.position = targetPosition;
        IsMoving = false;

        

        // After movement, check whether we've moved into an open Exit and delegate handling
        Collider2D[] hits = Physics2D.OverlapBoxAll(transform.position, Vector2.one * 0.9f, 0f);
        const float centerEpsilon = 0.15f;
        foreach (var h in hits)
        {
            if (Mathf.Abs(h.transform.position.x - transform.position.x) < centerEpsilon && Mathf.Abs(h.transform.position.y - transform.position.y) < centerEpsilon)
            {
                Exit exit = h.GetComponent<Exit>();
                if (exit != null && exit.IsOpen)
                {
                    LevelManager.Instance.OnPlayerEnterExit(exit);
                    yield break;
                }
            }
        }
    }
}
