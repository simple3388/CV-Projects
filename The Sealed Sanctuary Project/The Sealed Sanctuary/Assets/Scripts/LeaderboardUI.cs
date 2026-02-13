using UnityEngine;
using UnityEngine.UI;
using System.Collections.Generic;
using System.Text;

public class LeaderboardUI : MonoBehaviour
{
    public Text level1Text;
    public Text level2Text;
    public Font customFont;
    public int fontSize = 24;
    public Color textColor = Color.white;

    private void Start()
    {
        var leaderboard = SaveLoadManager.LoadLeaderboard();
        var level1Entries = leaderboard.FindAll(e => e.levelName == "Level1");
        var level2Entries = leaderboard.FindAll(e => e.levelName == "Level2");
        level1Entries.Sort((a, b) => a.moves.CompareTo(b.moves));
        level2Entries.Sort((a, b) => a.moves.CompareTo(b.moves));
        ShowLeaderboard(level1Text, level1Entries);
        ShowLeaderboard(level2Text, level2Entries);
    }

    private void ShowLeaderboard(Text textComponent, List<LeaderboardEntry> entries)
    {
        if (textComponent == null) return;

        if (customFont != null)
            textComponent.font = customFont;
        textComponent.fontSize = fontSize;
        textComponent.color = textColor;

        StringBuilder sb = new StringBuilder();
        sb.AppendLine();

        // Show only top 5 entries
        int count = Mathf.Min(entries.Count, 5);
        for (int i = 0; i < count; i++)
        {
            var entry = entries[i];
            sb.AppendLine($"{i + 1}. {entry.moves} moves");
            sb.AppendLine();
            sb.AppendLine();
        }

        if (count == 0)
            sb.AppendLine("No entries yet");

        textComponent.text = sb.ToString();
    }
}
