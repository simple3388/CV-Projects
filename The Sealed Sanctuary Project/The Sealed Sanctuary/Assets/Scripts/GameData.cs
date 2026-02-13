using UnityEngine;

public abstract class GameData : MonoBehaviour
{
    // Base class for game data scripts

    //Get Xmin, Xmax, Ymin, Ymax
    public static float XMin
    {
        get
        {
            return Camera.main.ScreenToWorldPoint(new Vector3(0, 0, 0)).x;
        }
    }
    public static float YMin
    {
        get
        {
            return Camera.main.ViewportToWorldPoint(new Vector3(0, 0, 0)).y;
        }
    }
    public static float XMax
    {
        get
        {
            return Camera.main.ViewportToWorldPoint(new Vector3(1, 0, 0)).x;
        }
    }
    public static float YMax
    {
        get
        {
            return Camera.main.ViewportToWorldPoint(new Vector3(0, 1, 0)).y;
        }
    }

    // Grid info (can be set by InitializeLevel at runtime)
    public static int GridCols { get; set; } = 12;
    public static int GridRows { get; set; } = 12;
    public static float GridStartX { get; set; } = -12f / 2f + 0.5f;
    public static float GridStartY { get; set; } = -12f / 2f + 0.5f;
    public static float TileSize { get; set; } = 1f;

    // Currently active level index (set by InitializeLevel)
    public static int CurrentLevelIndex { get; set; } = 1;

    // Global difficulty flag toggled from UI
    public static bool HardModeEnabled { get; set; } = false;
    public static int Level1MoveLimit { get; set; } = 100;
    public static int Level2MoveLimit { get; set; } = 100;
}
