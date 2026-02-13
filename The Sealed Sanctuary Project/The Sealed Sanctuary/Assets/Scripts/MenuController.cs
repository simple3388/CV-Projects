using UnityEngine;
using UnityEngine.SceneManagement;


using UnityEngine.UI;

public class MenuController : MonoBehaviour
{
    public string levelSelectSceneName = "LevelSelect";
    public string level1SceneName = "Level1";
    public string level2SceneName = "Level2";
    public string welcomeSceneName = "WelcomeScreen";
    public string leaderboardSceneName = "Leaderboard";

    // Hard Mode button UI
    public Sprite hardModeButtonSprite;
    public Sprite hardModeDownSprite;

    public Button hardModeButton;


    // Open level select screen from welcome menu
    public void OpenLevelSelect()
    {
        if (!string.IsNullOrEmpty(levelSelectSceneName))
            SceneManager.LoadScene(levelSelectSceneName);
    }


    // Called by a UI Toggle to enable/disable hard mode
    public void ToggleHardMode()
    {
        GameData.HardModeEnabled = !GameData.HardModeEnabled;
        if (GameData.HardModeEnabled && hardModeDownSprite != null)
            hardModeButton.image.sprite = hardModeDownSprite;
        else if (!GameData.HardModeEnabled && hardModeButtonSprite != null)
            hardModeButton.image.sprite = hardModeButtonSprite;
    }

    private void Start()
    {
        // Initialize button sprite based on current hard mode state
        if (GameData.HardModeEnabled && hardModeDownSprite != null)
            hardModeButton.image.sprite = hardModeDownSprite;
        else if (!GameData.HardModeEnabled && hardModeButtonSprite != null)
            hardModeButton.image.sprite = hardModeButtonSprite;
    }


    // Called by level buttons; set the current level index then load the game scene
    public void SelectLevel(int levelIndex)
    {
        GameData.CurrentLevelIndex = levelIndex;

        if(levelIndex == 1 && !string.IsNullOrEmpty(level1SceneName))
        {
            SceneManager.LoadScene(level1SceneName);
        }
        else if(levelIndex == 2 && !string.IsNullOrEmpty(level2SceneName))
        {
            SceneManager.LoadScene(level2SceneName);
        }
    }

    // Back button for level select to return to welcome screen
    public void BackToWelcome()
    {
        if (!string.IsNullOrEmpty(welcomeSceneName))
            SceneManager.LoadScene(welcomeSceneName);
    }

    // Quit button functionality 
    public void QuitGame()
    {
#if UNITY_EDITOR
        UnityEditor.EditorApplication.isPlaying = false;
#else
        Application.Quit();
#endif
    }

    // Go to leaderboard scene
    public void GoToLeaderboard()
    {
        SceneManager.LoadScene(leaderboardSceneName);
    }
}