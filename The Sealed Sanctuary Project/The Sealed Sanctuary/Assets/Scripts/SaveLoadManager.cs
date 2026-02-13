using System.Collections.Generic;
using UnityEngine;
using System.IO;

[System.Serializable]
public class LeaderboardEntry
{
    public int moves;
    public string levelName;
}

public class SaveLoadManager : MonoBehaviour
{
    private static string SavePath => Path.Combine(Application.persistentDataPath, "leaderboard.json");

    public static void SaveLeaderboard(List<LeaderboardEntry> leaderboard)
    {
        string json = JsonUtility.ToJson(new LeaderboardWrapper { entries = leaderboard }, true);
        File.WriteAllText(SavePath, json);
    }

    public static List<LeaderboardEntry> LoadLeaderboard()
    {
        if (!File.Exists(SavePath))
            return new List<LeaderboardEntry>();
        string json = File.ReadAllText(SavePath);
        return JsonUtility.FromJson<LeaderboardWrapper>(json).entries;
    }

    private class LeaderboardWrapper
    {
        public List<LeaderboardEntry> entries = new List<LeaderboardEntry>();
    }
}
