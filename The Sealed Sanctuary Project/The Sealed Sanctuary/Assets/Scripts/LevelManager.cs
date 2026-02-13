using UnityEngine;
using UnityEngine.SceneManagement;

public class LevelManager : Singleton<LevelManager>
{
    public string level1SceneName = "Level1";
    public string level2SceneName = "Level2";

    private string pendingScene = null;


    // Called when the player walks into an open exit
    public void OnPlayerEnterExit(Exit exit)
    {
        int exitLevel = 1;
        if (exit != null) exitLevel = exit.levelIndex;

        // Save moves to leaderboard
        var moveCounter = GameObject.FindObjectOfType<MoveCounterUI>();
        if (moveCounter != null)
        {
            var leaderboard = SaveLoadManager.LoadLeaderboard();
            string levelName = exitLevel == 1 ? level1SceneName : level2SceneName;
            leaderboard.Add(new LeaderboardEntry {
                moves = moveCounter.GetMoveCount(),
                levelName = levelName
            });
            SaveLoadManager.SaveLeaderboard(leaderboard);
        }

        if (exitLevel == 1)
        {
            GoToScene(level2SceneName, 2);
        }
        else if (exitLevel == 2)
        {
            GoToWinScene();
        }
    }

    // Load a specific scene name. Optionally set the GameData level index.
    public void GoToScene(string sceneName, int levelIndex = 0)
    {
        if (levelIndex > 0) GameData.CurrentLevelIndex = levelIndex;
        // Pass the target scene name to OnSceneLoaded for cleanup
        pendingScene = sceneName;
        SceneManager.sceneLoaded += OnSceneLoaded;
        SceneManager.LoadScene(sceneName);
    }

    private void OnSceneLoaded(Scene scene, LoadSceneMode mode)
    {
        // Desroy InitializeLevel if not going to a level scene
        if (pendingScene != level1SceneName && pendingScene != level2SceneName)
        {
            var initLevel = InitializeLevel.Instance;
            if (initLevel != null)
            {
                Destroy(initLevel.gameObject);
            }
        }
        pendingScene = null;

        var moveCounter = GameObject.FindObjectOfType<MoveCounterUI>();
        if (moveCounter != null)
            moveCounter.ResetMoveCount();
        SceneManager.sceneLoaded -= OnSceneLoaded;
    }

    public void GoToWinScene()
    {
        // Use the same cleanup logic as GoToScene
        pendingScene = "WinGame";
        SceneManager.sceneLoaded += OnSceneLoaded;
        SceneManager.LoadScene("WinGame");
    }

    public void GoToLoseScene()
    {
        // Use the same cleanup logic as GoToScene
        pendingScene = "LoseGame";
        SceneManager.sceneLoaded += OnSceneLoaded;
        SceneManager.LoadScene("LoseGame");
    }
}
