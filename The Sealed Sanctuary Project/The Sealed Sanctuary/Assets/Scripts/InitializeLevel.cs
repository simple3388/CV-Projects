
using UnityEngine;
using UnityEngine.SceneManagement;

public class InitializeLevel : Singleton<InitializeLevel>
{
    public float padding;
    private GameObject [,] grid;
    private GameObject gridContainer;

    [SerializeField] public GameObject groundTilePrefab;
    [SerializeField] public GameObject wallPrefab;
    [SerializeField] public GameObject firePrefab;
    [SerializeField] public GameObject keystonePrefab;
    [SerializeField] public GameObject waterPotPrefab;
    [SerializeField] public GameObject exitPrefab;
    [SerializeField] public GameObject goalPrefab;
    [SerializeField] public int levelIndex = 1; // 1 or 2

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Start()
    {
        // Auto-detect level index from the scene name (helps when starting directly in Level2)
        UpdateLevelIndexFromScene(SceneManager.GetActiveScene());
    }

    private void OnEnable()
    {
        SceneManager.sceneLoaded += OnSceneLoaded;
    }

    private void OnDisable()
    {
        SceneManager.sceneLoaded -= OnSceneLoaded;
    }

    private void OnSceneLoaded(Scene scene, LoadSceneMode mode)
    {
        // When a new scene loads, try to detect the appropriate level index from the scene
        UpdateLevelIndexFromScene(scene);
        levelIndex = GameData.CurrentLevelIndex;
        drawGrid();
        spawnPlayer();
    }

    private void UpdateLevelIndexFromScene(Scene scene)
    {
        string name = scene.name;
        var lm = LevelManager.Instance;

        if (lm == null) return; // LevelManager not available yet

        // set level index based on scene name in order to load correct layout
        if (name == lm.level2SceneName)
        {
            GameData.CurrentLevelIndex = 2;
            return;
        }
        else if (name == lm.level1SceneName)
        {
            GameData.CurrentLevelIndex = 1;
            return;
        }
    }


    void drawGrid()
    {
        // Clear previous grid objects (if any)
        if (gridContainer != null)
        {
            Destroy(gridContainer);
        }

        //create grid container and set as child of this object
        gridContainer = new GameObject("LevelGrid");
        gridContainer.transform.SetParent(transform);

        float startX = -12 / 2f + 0.5f + padding;
        float startY = -12 / 2f + 0.5f + padding;
        grid = new GameObject[12, 12];



        // register grid origin and size in GameData so movement bounds use exact grid
        GameData.GridStartX = startX;
        GameData.GridStartY = startY;
        GameData.GridCols = grid.GetLength(1);
        GameData.GridRows = grid.GetLength(0);

        // get layout for selected level
        // record current level index for other systems
        GameData.CurrentLevelIndex = levelIndex;
        char[,] layout = LevelDefinitions.GetLayout(levelIndex);


        for (int r = 0; r < 12; r++)
        {
            for (int c = 0; c < 12; c++)
            {
                Vector3 position = new Vector3(startX + c, startY + r, 0f);
                grid[r, c] = Instantiate(groundTilePrefab, position, Quaternion.identity, gridContainer.transform);

                // spawn overlay according to layout (walls, fires, pushables)
                char tile = layout[r, c];
                Vector3 overlayPos = position + new Vector3(0f, 0f, -1f);
                switch (tile)
                {
                    case '#':
                        if (wallPrefab != null) Instantiate(wallPrefab, overlayPos, Quaternion.identity, gridContainer.transform);
                        break;
                    case 'F':
                        if (firePrefab != null)
                        {
                            GameObject f = Instantiate(firePrefab, overlayPos, Quaternion.identity, gridContainer.transform);
                            if (f.GetComponent<Fire>() == null) f.AddComponent<Fire>();
                        }
                        break;
                    case 'K':
                        if (keystonePrefab != null)
                        {
                            GameObject k = Instantiate(keystonePrefab, overlayPos, Quaternion.identity, gridContainer.transform);
                            if (k.GetComponent<Pushable>() == null) k.AddComponent<Pushable>();
                            var push = k.GetComponent<Pushable>();
                            if (push != null) push.isKeystone = true;
                        }
                        break;
                    case 'W':
                        if (waterPotPrefab != null)
                        {
                            GameObject p2 = Instantiate(waterPotPrefab, overlayPos, Quaternion.identity, gridContainer.transform);
                            if (p2.GetComponent<Pushable>() == null) p2.AddComponent<Pushable>();
                        }
                        break;
                    case 'G':
                        if (goalPrefab != null)
                        {
                            GameObject g = Instantiate(goalPrefab, overlayPos, Quaternion.identity, gridContainer.transform);
                        }
                        break;
                    case 'E':
                        if (exitPrefab !=null)
                        {
                            GameObject e = Instantiate(exitPrefab, overlayPos, Quaternion.identity, gridContainer.transform);
                            Exit ex = e.GetComponent<Exit>();
                            if (ex == null) ex = e.AddComponent<Exit>();
                            ex.levelIndex = levelIndex;

                        }
                        break;
                    default:
                        break;
                }
            }
        }
    }
    
    void spawnPlayer()
    {
        GameObject playerPrefab = Resources.Load("Player") as GameObject;
        // Spawn the player at the middle of the bottom-centre tile of the grid
        if (grid != null && grid.GetLength(0) > 0 && grid.GetLength(1) > 0)
        {
            int centerCol = grid.GetLength(1) / 2; // divide length of columns by 2 to get center column
            GameObject bottomCenterTile = grid[0, centerCol]; // get position of bottom center tile as row 0 is bottom
            Vector3 spawnPos = new Vector3(0f, 0f, -1f); 
            if (bottomCenterTile != null) //check bottom center tile is not null
            {
                spawnPos = bottomCenterTile.transform.position + new Vector3(0f, 0f, -2f);
            }
            // Parent the player to gridContainer so it is destroyed with InitializeLevel in level manager
            Instantiate(playerPrefab, spawnPos, Quaternion.identity, gridContainer != null ? gridContainer.transform : null);
        }
    }


}
