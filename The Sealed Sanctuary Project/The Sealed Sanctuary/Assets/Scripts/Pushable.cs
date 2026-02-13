using UnityEngine;

public class Pushable : MonoBehaviour
{
    public PushableData data;
    public bool isKeystone = false;
    public bool isLocked = false;

    // Try to push this object one tile in `dir`. Uses the caller's blocking layer mask.
    // Returns true if the push succeeded (object moved / extinguished fire).
    public bool TryPush(Vector2Int dir, LayerMask blockingLayer)
    {
        // Check if keystone has been moved onto goal and is now locked
        if (isLocked) return false;

        Vector3 destination = transform.position + new Vector3(dir.x, dir.y, 0f);

        int destCol = Mathf.RoundToInt(destination.x - GameData.GridStartX);
        int destRow = Mathf.RoundToInt(destination.y - GameData.GridStartY);

        // check bounds
        if (destCol < 0 || destCol >= GameData.GridCols || destRow < 0 || destRow >= GameData.GridRows) // out of bounds
            return false;

        Collider2D hit = null;
        Collider2D[] hits = Physics2D.OverlapBoxAll(destination, Vector2.one * 0.9f, 0f, blockingLayer);

        //check if there is an obstacle exactly at the destination 
        const float centerEpsilon = 0.15f;
        foreach (var h in hits)
        {
            if (Mathf.Abs(h.transform.position.x - destination.x) < centerEpsilon && Mathf.Abs(h.transform.position.y - destination.y) < centerEpsilon)
            {
                hit = h;
                break;
            }
        }

        //check if obstacle is a fire that can be extinguished
        if (hit != null)
        {
            // If the thing blocking is a fire and this pushable is a water pot, extinguish then occupy the tile
            Fire fire = hit.GetComponent<Fire>();
            if (fire != null && data != null && data.extinguishesFire)
            {
                fire.Extinguish();
                Destroy(gameObject);
                return true;
            }

            return false; // blocked by something else
        }

        // No blocker — move into the tile
        transform.position = destination;

        // If this is a keystone, check whether it was moved onto a goal tile ('G')
        if (isKeystone)
        {
            // get the layout for the current level and see if destination is a goal
            char[,] layout = LevelDefinitions.GetLayout(GameData.CurrentLevelIndex);
            if (destRow >= 0 && destRow < GameData.GridRows && destCol >= 0 && destCol < GameData.GridCols) // double-check bounds
            {
                if (layout[destRow, destCol] == 'G')
                {
                    // align keystone position on center and in front of goal tile
                    transform.position = destination + new Vector3(0f, 0.1f, -0.5f);
                    OpenExit();
                    // lock the keystone so it cannot be moved again
                    isLocked = true;
                }
            }
        }

        return true;
    }

    private void OpenExit()
    {
        Exit exit = FindObjectOfType<Exit>();
        if (exit != null)
        {
            exit.Open();
        }
    }
}
