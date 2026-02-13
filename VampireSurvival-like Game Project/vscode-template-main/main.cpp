#include "raylib.h"
#include <string>
#include <cmath>
#include <vector>

using namespace std;

// Constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int UI_HEIGHT = 80;
const int GAME_AREA_HEIGHT = SCREEN_HEIGHT - UI_HEIGHT; 

// Forward Declarations
class Enemy;
class Player;
class Weapon;
class WeaponDrop;
class Projectile;
class ExperienceChest;

// Weapon Types Enum
enum WeaponType {
    WEAPON_SWORD,   // Low range, high damage
    WEAPON_BOW,     // Medium range, medium damage
    WEAPON_STAFF    // High range, low damage
};


// Upgrade Types Enum
enum UpgradeType {
    UPGRADE_DAMAGE,
    UPGRADE_HEALTH,
    UPGRADE_SPEED
};

// Enemy Types Enum
enum EnemyType {
    ENEMY_TANK,      // Slow speed, high health, high damage
    ENEMY_BALANCED,  // Medium speed, medium health, medium damage
    ENEMY_RUNNER     // High speed, low health, low damage
};

// Projectile/Attack Class
class Projectile {
public:
    Vector2 position;
    Vector2 velocity;
    float radius;
    int damage;
    Color color;
    bool active;
    WeaponType type;
    
    Projectile(Vector2 pos, Vector2 vel, int dmg, Color col, WeaponType wType) {
        position = pos;
        velocity = vel;
        damage = dmg;
        color = col;
        radius = 5.0f;
        active = true;
        type = wType;
    }
    
    void Update(float deltaTime) {
        // Move projectile
        position.x += velocity.x * deltaTime;
        position.y += velocity.y * deltaTime;
        
        // Deactivate if out of bounds
        if (position.x < 0 || position.x > SCREEN_WIDTH || 
            position.y < 0 || position.y > GAME_AREA_HEIGHT) {
            active = false;
        }
    }
    
    void Draw() {
        if (active) {
            // Sword projectiles are invisible (melee-like)
            if (type == WEAPON_SWORD) {
                return; // Don't draw anything
            }
            else if (type == WEAPON_STAFF) {
                // Draw projectile
                DrawCircleV(position, radius + 2, color);
                DrawCircleV(position, radius, ColorAlpha(WHITE, 0.8f));
            } else {
                DrawCircleV(position, radius, color);
            }
        }
    }
};


// Enemy Class
class Enemy {
public:
    Vector2 position;
    float radius;
    float speed;
    int health;
    int maxHealth;
    int damage;
    bool active;
    EnemyType type;
    Texture2D* texture;
    
    Enemy(Vector2 pos, EnemyType enemyType, Texture2D* tex = nullptr) {
        position = pos;
        type = enemyType;
        active = true;
        texture = tex;
        
        // Set stats based on enemy type
        switch(type) {
            case ENEMY_TANK:
                // Type 1: Slow, high health, high damage
                speed = 50.0f;
                maxHealth = 100;
                damage = 20;
                radius = 20.0f;  // Larger size
                break;
                
            case ENEMY_BALANCED:
                // Type 2: Medium stats
                speed = 80.0f;
                maxHealth = 50;
                damage = 10;
                radius = 15.0f;
                break;
                
            case ENEMY_RUNNER:
                // Type 3: Fast, low health, low damage
                speed = 120.0f;
                maxHealth = 25;
                damage = 5;
                radius = 18.0f;
                break;
        }
        
        health = maxHealth;
    }
    
    void Update(float deltaTime, Vector2 playerPos) {
        if (!active) return;
        
        // Move towards player
        Vector2 direction = {
            playerPos.x - position.x,
            playerPos.y - position.y
        };
        
        float length = sqrt(direction.x * direction.x + direction.y * direction.y);
        if (length > 0) {
            direction.x /= length;
            direction.y /= length;
        }
        
        position.x += direction.x * speed * deltaTime;
        position.y += direction.y * speed * deltaTime;
    }
    
    void Draw() {
        if (active) {
            if (texture != nullptr && texture->id > 0) {
                // Draw enemy sprite centered on position
                float scale = (radius * 2.0f) / texture->width;
                Vector2 drawPos = {
                    position.x - (texture->width * scale) / 2,
                    position.y - (texture->height * scale) / 2
                };
                DrawTextureEx(*texture, drawPos, 0.0f, scale, WHITE);
            }
            
            // Health bar above enemy
            float healthPercent = (float)health / (float)maxHealth;
            int barWidth = 30;
            int barHeight = 4;
            Vector2 barPos = {position.x - barWidth/2, position.y - radius - 8};
            
            DrawRectangle((int)barPos.x, (int)barPos.y, barWidth, barHeight, DARKGRAY);
            DrawRectangle((int)barPos.x, (int)barPos.y, (int)(barWidth * healthPercent), barHeight, GREEN);
        }
    }
    
    void TakeDamage(int damage) {
        health -= damage;
        if (health <= 0) {
            active = false;
        }
    }
    
    bool CheckCollision(Vector2 point, float pointRadius) {
        float dx = position.x - point.x;
        float dy = position.y - point.y;
        float distance = sqrt(dx * dx + dy * dy);
        return distance < (radius + pointRadius);
    }
    
    int GetDamage() {
        return damage;
    }
};

// Weapon Class
class Weapon {
public:
    WeaponType type;
    float range;
    int damage;
    float fireRate;
    float fireTimer;
    Color projectileColor;
    string name;
    Texture2D* texture;
    
    Weapon(WeaponType wType, Texture2D* tex = nullptr) {
        type = wType;
        fireTimer = 0.0f;
        texture = tex;
        
        // Set weapon stats based on type
        switch(type) {
            case WEAPON_SWORD:
                name = "Sword";
                range = 100.0f;      // Low range
                damage = 35;         // High damage
                fireRate = 0.8f;     // Attacks per second
                projectileColor = LIGHTGRAY;
                break;
            case WEAPON_BOW:
                name = "Bow";
                range = 250.0f;      // Medium range
                damage = 20;         // Medium damage
                fireRate = 1.2f;
                projectileColor = BROWN;
                break;
            case WEAPON_STAFF:
                name = "Staff";
                range = 400.0f;      // High range
                damage = 12;         // Low damage
                fireRate = 2.0f;     // Fast attacks
                projectileColor = PURPLE;
                break;
        }
    }
    
    void Update(float deltaTime) {
        fireTimer += deltaTime; // Update time since last shot
    }
    
    void DrawWeapon(Vector2 playerPos, float playerRadius) {
        if (texture != nullptr && texture->id > 0) {
            // Draw weapon sprite next to player (right side)
            float weaponSize = playerRadius * 1.5f;
            float scale = weaponSize / texture->width;
            Vector2 weaponOffset = {playerRadius + 10, 0}; // Offset to the right
            
            Vector2 drawPos = {
                playerPos.x + weaponOffset.x - (texture->width * scale) / 2,
                playerPos.y + weaponOffset.y - (texture->height * scale) / 2
            };
            
            DrawTextureEx(*texture, drawPos, 0.0f, scale, WHITE);
        }
    }
    
    bool CanFire() {
        return fireTimer >= (1.0f / fireRate);
    }
    
    void ResetFireTimer() {
        fireTimer = 0.0f;
    }
    
    //Target nearby enemies
    Enemy* FindNearestEnemy(Vector2 playerPos, vector<Enemy*>& enemies) {
        Enemy* nearest = nullptr;
        float nearestDist = range;
        
        for (Enemy* enemy : enemies) { // loop through enemies to find nearest
            if (!enemy->active) continue; // Skip inactive enemies
            
            // Calculate distance to enemy  
            float dx = enemy->position.x - playerPos.x;
            float dy = enemy->position.y - playerPos.y;
            float dist = sqrt(dx * dx + dy * dy);
            
            if (dist < nearestDist) {
                nearestDist = dist;
                nearest = enemy;
            }
        }
        
        return nearest;
    }
    
    // Fire weapon automatically toward enemy
    Projectile* Fire(Vector2 playerPos, Enemy* target, float damageMultiplier = 1.0f) {
        if (target == nullptr) return nullptr;  // prevents crash if no target within range
        
        // Calculate direction to enemy
        Vector2 direction = {
            target->position.x - playerPos.x,
            target->position.y - playerPos.y
        };

        // Normalize direction
        float length = sqrt(direction.x * direction.x + direction.y * direction.y);
        if (length > 0) {
            direction.x /= length;
            direction.y /= length;
        }
        
        // Set projectile speed based on weapon type
        float projectileSpeed = (type == WEAPON_STAFF) ? 500.0f : 
                               (type == WEAPON_BOW) ? 400.0f : 350.0f;
        
        Vector2 velocity = {
            direction.x * projectileSpeed,
            direction.y * projectileSpeed
        };
        
        // Apply damage multiplier
        int finalDamage = (int)(damage * damageMultiplier);
        
        return new Projectile(playerPos, velocity, finalDamage, projectileColor, type); // Create and return new projectile
    }
};


// Experience Chest Class
class ExperienceChest {
public:
    Vector2 position;
    float radius;
    bool collected;
    Color color;
    Texture2D* texture;
    int xpAmount;
    
    ExperienceChest(Vector2 pos, Texture2D* tex = nullptr, int xp = 10) {
        position = pos;
        radius = 12.0f;
        collected = false;
        texture = tex;
        xpAmount = xp;
        color = YELLOW;
    }
    
    void Draw() {
        if (!collected) {
            if (texture != nullptr && texture->id > 0) {
                // Draw XP chest sprite
                float scale = (radius * 2.0f / texture->width);
                
                Vector2 drawPos = {
                    position.x - (texture->width * scale) / 2,
                    position.y - (texture->height * scale) / 2
                };
                DrawTextureEx(*texture, drawPos, 0.0f, scale, WHITE);
            }
        }
    }
    
    bool CheckCollision(Vector2 playerPos, float playerRadius) {
        if (collected) return false;
        
        float dx = position.x - playerPos.x;
        float dy = position.y - playerPos.y;
        float distance = sqrt(dx * dx + dy * dy);
        return distance < (radius + playerRadius);
    }
};


// Weapon Drop Class
class WeaponDrop {
public: 
    Vector2 position;
    WeaponType type;
    float radius;
    bool collected;
    Texture2D* texture;
    
    WeaponDrop(Vector2 pos, WeaponType wType, Texture2D* tex = nullptr) {
        position = pos;
        type = wType;
        radius = 15.0f;
        collected = false;
        texture = tex;
    }
    
    void Draw() {
        if (!collected) {
            if (texture != nullptr && texture->id > 0) {
                float scale = (radius * 2.0f / texture->width);
            
                // Draw weapon sprite centered
                Vector2 drawPos = {
                    position.x - (texture->width * scale) / 2,
                    position.y - (texture->height * scale) / 2
                };
                DrawTextureEx(*texture, drawPos, 0.0f, scale, WHITE);
            }
        }
    }
    
    bool CheckCollision(Vector2 playerPos, float playerRadius) {
        if (collected) return false;
        
        float dx = position.x - playerPos.x;
        float dy = position.y - playerPos.y;
        float distance = sqrt(dx * dx + dy * dy);
        return distance < (radius + playerRadius);
    }
};


class Player {
public:
    Vector2 position;
    float radius;
    float speed;
    Texture2D* texture;
    
    // Weapon
    Weapon* currentWeapon;
    
    // Stats
    int maxHealth;
    int currentHealth;
    int level;
    int currentExp;
    int expToNextLevel;
    float baseDamageMultiplier; // Affects weapon damage
    
    // Constructor
    Player(Vector2 pos, float r, float spd, Texture2D* tex = nullptr) {
        position = pos;
        radius = r;
        speed = spd;
        texture = tex;
        
        //Start with one weapon
        currentWeapon = nullptr; // Will be set when textures are loaded
        
        // Initialize stats
        maxHealth = 100;
        currentHealth = maxHealth;
        level = 1;
        currentExp = 0;
        expToNextLevel = 100;
        baseDamageMultiplier = 1.0f;
    }
    
    void Update(float deltaTime) {
        // Movement input
        Vector2 movement = {0, 0};
        
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) movement.y -= 1;
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) movement.y += 1;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) movement.x -= 1;
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) movement.x += 1;
        
        // Normalize diagonal movement
        float length = sqrt(movement.x * movement.x + movement.y * movement.y);
        if (length > 0) {
            movement.x /= length;
            movement.y /= length;
        }
        
        // Apply movement
        position.x += movement.x * speed * deltaTime;
        position.y += movement.y * speed * deltaTime;
        
        // Keep player within game area bounds
        if (position.x - radius < 0) position.x = radius;
        if (position.x + radius > SCREEN_WIDTH) position.x = SCREEN_WIDTH - radius;
        if (position.y - radius < 0) position.y = radius;
        if (position.y + radius > GAME_AREA_HEIGHT) position.y = GAME_AREA_HEIGHT - radius;
    }
    
    void Draw() {
        if (texture != nullptr && texture->id > 0) {
            // Draw player sprite centered on position
            float scale = (radius * 3.0f) / texture->width; // Scale based on radius
            Vector2 drawPos = {
                position.x - (texture->width * scale) /2,
                position.y - (texture->height * scale) /2,
            };
            DrawTextureEx(*texture, drawPos, 0.0f, scale, WHITE);
        }
        
        // Draw equipped weapon sprite
        if (currentWeapon != nullptr) {
            currentWeapon->DrawWeapon(position, radius);
        }
    }
    
    bool AddExp(int amount) {
        currentExp += amount;
        if (currentExp >= expToNextLevel) {
            return LevelUp(); // Return true if leveled up
        }
        return false;
    }
    
    bool LevelUp() {
        level++;
        currentExp -= expToNextLevel;
        expToNextLevel = 100;
        return true; // Return true to trigger upgrade screen
    }
    
    void ApplyUpgrade(UpgradeType upgrade) {
        switch(upgrade) {
            case UPGRADE_DAMAGE:
                baseDamageMultiplier += 0.2f; // +20% damage
                break;
            case UPGRADE_HEALTH:
                maxHealth += 20; // +20 max health
                currentHealth += 20; // Also heal the added health
                break;
            case UPGRADE_SPEED:
                speed += 30.0f; // +30 speed
                break;
        }
    }
    
    void TakeDamage(int damage) {
        currentHealth -= damage;
        if (currentHealth < 0) currentHealth = 0;
    }
    
    bool IsAlive() {
        return currentHealth > 0;
    }
    
    void SetWeapon(Weapon* newWeapon) {
        // Replace current weapon with new one
        if (currentWeapon != nullptr) {
            delete currentWeapon;
        }
        currentWeapon = newWeapon;
    }
};


// UI Rendering Functions
void DrawGameUI(Player* player, float survivalTime) {
    // UI Background
    DrawRectangle(0, GAME_AREA_HEIGHT, SCREEN_WIDTH, UI_HEIGHT, ColorAlpha(BLACK, 0.8f));
    DrawRectangle(0, GAME_AREA_HEIGHT, SCREEN_WIDTH, 2, GOLD);
    
    int uiY = GAME_AREA_HEIGHT + 10;
    int leftMargin = 20;
    
    // Health Bar 
    DrawText("HEALTH", leftMargin, uiY, 16, WHITE);
    int healthBarWidth = 200;
    int healthBarHeight = 20;
    int healthBarX = leftMargin;
    int healthBarY = uiY + 20;
    
    // Health bar background
    DrawRectangle(healthBarX, healthBarY, healthBarWidth, healthBarHeight, DARKGRAY);
    
    // Current health
    float healthPercent = (float)player->currentHealth / (float)player->maxHealth;
    int currentHealthWidth = (int)(healthBarWidth * healthPercent);
    Color healthColor = GREEN;
    if (healthPercent < 0.5f) healthColor = YELLOW;
    if (healthPercent < 0.25f) healthColor = RED;
    
    DrawRectangle(healthBarX, healthBarY, currentHealthWidth, healthBarHeight, healthColor);
    DrawRectangleLines(healthBarX, healthBarY, healthBarWidth, healthBarHeight, WHITE);
    
    // Health text inside bar
    string healthText = to_string(player->currentHealth) + "/" + to_string(player->maxHealth);
    DrawText(healthText.c_str(), healthBarX + 5, healthBarY + 3, 14, WHITE);
    
    // Experience Bar
    int expBarX = leftMargin + 250;
    DrawText("EXPERIENCE", expBarX, uiY, 16, WHITE);
    int expBarWidth = 250;
    int expBarHeight = 20;
    int expBarY = uiY + 20;
    
    // Exp bar background
    DrawRectangle(expBarX, expBarY, expBarWidth, expBarHeight, DARKGRAY);
    
    // Current experience
    float expPercent = (float)player->currentExp / (float)player->expToNextLevel;
    int currentExpWidth = (int)(expBarWidth * expPercent);
    DrawRectangle(expBarX, expBarY, currentExpWidth, expBarHeight, PURPLE);
    DrawRectangleLines(expBarX, expBarY, expBarWidth, expBarHeight, WHITE);
    
    // Exp text
    string expText = to_string(player->currentExp) + "/" + to_string(player->expToNextLevel);
    DrawText(expText.c_str(), expBarX + 5, expBarY + 3, 14, WHITE);
    
    // Level Display
    int levelX = leftMargin + 550;
    DrawText("LEVEL", levelX, uiY, 16, GOLD);
    string levelText = to_string(player->level);
    DrawText(levelText.c_str(), levelX + 10, uiY + 22, 24, GOLD);
    
    // Timer Display
    int timerX = SCREEN_WIDTH - 200;
    DrawText("TIME", timerX, uiY, 16, SKYBLUE);
    int minutes = (int)survivalTime / 60;
    int seconds = (int)survivalTime % 60;
    string timerText = (minutes < 10 ? "0" : "") + to_string(minutes) + ":" + 
                           (seconds < 10 ? "0" : "") + to_string(seconds);
    DrawText(timerText.c_str(), timerX, uiY + 22, 24, SKYBLUE);
}

void DrawUpgradeScreen(int mouseX, int mouseY) {
    // Semi-transparent overlay
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.7f));
    
    // Title
    const char* title = "LEVEL UP!";
    int titleWidth = MeasureText(title, 60);
    DrawText(title, SCREEN_WIDTH/2 - titleWidth/2, 150, 60, GOLD);
    
    const char* subtitle = "Choose an upgrade:";
    int subtitleWidth = MeasureText(subtitle, 30);
    DrawText(subtitle, SCREEN_WIDTH/2 - subtitleWidth/2, 230, 30, WHITE);
    
    // Upgrade options
    int boxWidth = 300;
    int boxHeight = 150;
    int spacing = 50;
    int startX = SCREEN_WIDTH/2 - (boxWidth * 3 + spacing * 2) / 2; // Center the three boxes based on total width
    int boxY = 320;
    
    Rectangle damageBox = {(float)startX, (float)boxY, (float)boxWidth, (float)boxHeight};
    Rectangle healthBox = {(float)(startX + boxWidth + spacing), (float)boxY, (float)boxWidth, (float)boxHeight};
    Rectangle speedBox = {(float)(startX + (boxWidth + spacing) * 2), (float)boxY, (float)boxWidth, (float)boxHeight};
    
    // Check mouse hover
    bool hoverDamage = CheckCollisionPointRec({(float)mouseX, (float)mouseY}, damageBox);
    bool hoverHealth = CheckCollisionPointRec({(float)mouseX, (float)mouseY}, healthBox);
    bool hoverSpeed = CheckCollisionPointRec({(float)mouseX, (float)mouseY}, speedBox);
    
    // Draw Damage upgrade
    DrawRectangleRec(damageBox, hoverDamage ? DARKGRAY : Fade(RED, 0.3f)); // fill rectangle with a transparent color if not hovered
    DrawRectangleLinesEx(damageBox, 3, hoverDamage ? WHITE : RED); // border
    DrawText("MORE DAMAGE", damageBox.x + 40, damageBox.y + 30, 24, WHITE);
    DrawText("+20% Damage", damageBox.x + 65, damageBox.y + 70, 20, LIGHTGRAY);
    
    // Draw Health upgrade
    DrawRectangleRec(healthBox, hoverHealth ? DARKGRAY : Fade(GREEN, 0.3f));
    DrawRectangleLinesEx(healthBox, 3, hoverHealth ? WHITE : GREEN);
    DrawText("MORE HEALTH", healthBox.x + 45, healthBox.y + 30, 24, WHITE);
    DrawText("+20 Max HP", healthBox.x + 70, healthBox.y + 70, 20, LIGHTGRAY);
    
    // Draw Speed upgrade
    DrawRectangleRec(speedBox, hoverSpeed ? DARKGRAY : Fade(SKYBLUE, 0.3f));
    DrawRectangleLinesEx(speedBox, 3, hoverSpeed ? WHITE : SKYBLUE);
    DrawText("MORE SPEED", speedBox.x + 55, speedBox.y + 30, 24, WHITE);
    DrawText("+30 Speed", speedBox.x + 80, speedBox.y + 70, 20, LIGHTGRAY);
    
    // Instructions
    const char* instruction = "Click to select an upgrade";
    int instructionWidth = MeasureText(instruction, 20);
    DrawText(instruction, SCREEN_WIDTH/2 - instructionWidth/2, 520, 20, LIGHTGRAY);
}

void DrawBackground(Texture2D* floorTexture) {
    if (floorTexture != nullptr && floorTexture->id > 0) {
        // Draw tiled floor texture
        int tileWidth = floorTexture->width;
        int tileHeight = floorTexture->height;
        
        // Calculate how many tiles are needed
        int tilesX = (SCREEN_WIDTH / tileWidth) + 1;
        int tilesY = (GAME_AREA_HEIGHT / tileHeight) + 1;
        
        // Draw tiles to fill the screen
        for (int y = 0; y < tilesY; y++) {
            for (int x = 0; x < tilesX; x++) {
                DrawTexture(*floorTexture, x * tileWidth, y * tileHeight, WHITE);
            }
        }
    }
}


// Program main entry point
int main(void)
{
    // Initialization
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Survivor Game - Top-Down Action");
    InitAudioDevice();
    SetTargetFPS(60);
    
    // Load floor texture
    Texture2D floorTexture = {0};
    const char* floorTexturePath = "Resources/FloorTile.png";
    
    if (FileExists(floorTexturePath)) {
        floorTexture = LoadTexture(floorTexturePath);
    }
    
    // Load player texture
    Texture2D playerTexture = {0};
    const char* playerTexturePath = "Resources/Player.png";
    
    if (FileExists(playerTexturePath)) {
        playerTexture = LoadTexture(playerTexturePath);
    }
    
    // Load weapon textures
    Texture2D swordTexture = {0};
    Texture2D bowTexture = {0};
    Texture2D staffTexture = {0};
    
    const char* swordPath = "Resources/Sword.png";
    const char* bowPath = "Resources/Bow.png";
    const char* staffPath = "Resources/Staff.png";
    
    if (FileExists(swordPath)) {
        swordTexture = LoadTexture(swordPath);
    } 
    
    if (FileExists(bowPath)) {
        bowTexture = LoadTexture(bowPath);
    }
    
    if (FileExists(staffPath)) {
        staffTexture = LoadTexture(staffPath);
    }

    // Load enemy textures
    Texture2D enemyTankTexture = {0};
    Texture2D enemyBalancedTexture = {0};
    Texture2D enemyRunnerTexture = {0};
    
    const char* tankPath = "Resources/KingSlime.png";
    const char* balancedPath = "Resources/slime.png";
    const char* runnerPath = "Resources/Zombie.png";
    
    if (FileExists(tankPath)) {
        enemyTankTexture = LoadTexture(tankPath);
    }
    
    if (FileExists(balancedPath)) {
        enemyBalancedTexture = LoadTexture(balancedPath);
    }
    
    if (FileExists(runnerPath)) {
        enemyRunnerTexture = LoadTexture(runnerPath);
    }
    
    // Load experience box texture
    Texture2D xpChest = {0};
    const char* xpChestPath = "Resources/Chest.png";
    
    if (FileExists(xpChestPath)) {
        xpChest = LoadTexture(xpChestPath);
    }
    
    // Load audio
    Music backgroundMusic = {0};
    Sound hitSound = {0};
    Sound levelUpSound = {0};
    Sound enemyDeathSound = {0};
    Sound winSound = {0};
    
    const char* musicPath = "Resources/Backround.mp3";
    const char* hitSoundPath = "Resources/PlayerDamage.mp3";
    const char* levelUpSoundPath = "Resources/LevelUp.wav";
    const char* enemyDeathPath = "Resources/EnemyDeath.mp3";
    const char* winSoundPath = "Resources/LevelWin.wav";
    
    if (FileExists(musicPath)) {
        backgroundMusic = LoadMusicStream(musicPath);
        SetMusicVolume(backgroundMusic, 0.5f);
    }
    
    if (FileExists(hitSoundPath)) {
        hitSound = LoadSound(hitSoundPath);
        SetSoundVolume(hitSound, 0.5f);
    }
    
    if (FileExists(levelUpSoundPath)) {
        levelUpSound = LoadSound(levelUpSoundPath);
        SetSoundVolume(levelUpSound, 0.6f);
    }
    
    if (FileExists(enemyDeathPath)) {
        enemyDeathSound = LoadSound(enemyDeathPath);
        SetSoundVolume(enemyDeathSound, 0.4f);
    }
    
    if (FileExists(winSoundPath)) {
        winSound = LoadSound(winSoundPath);
        SetSoundVolume(winSound, 0.7f);
    }

    // Create player at center of screen
    Vector2 playerStartPos = {SCREEN_WIDTH / 2.0f, GAME_AREA_HEIGHT / 2.0f};
    Player* player = new Player(playerStartPos, 20.0f, 200.0f, &playerTexture);
    
    // Give player starting weapon
    player->SetWeapon(new Weapon(WEAPON_SWORD, &swordTexture));
    
    // Game vectors for entities
    vector<Enemy*> enemies;
    vector<Projectile*> projectiles;
    vector<WeaponDrop*> weaponDrops;
    vector<ExperienceChest*> experienceChests;
    
    // Enemy spawn system
    float enemySpawnTimer = 0.0f;
    float enemySpawnRate = 2.0f; // Spawn every 2 seconds
    
    //Spawn initial weapon drops (randomly placed)
    for (int i = 0; i < 5; i++) {
        WeaponType randomType = (WeaponType)(GetRandomValue(0, 2));
        Vector2 randomPos = {
            (float)GetRandomValue(100, SCREEN_WIDTH - 100),
            (float)GetRandomValue(100, GAME_AREA_HEIGHT - 100)
        };
        
        // Select appropriate texture
        Texture2D* dropTexture = nullptr;
        switch(randomType) {
            case WEAPON_SWORD: dropTexture = &swordTexture; break;
            case WEAPON_BOW: dropTexture = &bowTexture; break;
            case WEAPON_STAFF: dropTexture = &staffTexture; break;
        }
        
        weaponDrops.push_back(new WeaponDrop(randomPos, randomType, dropTexture));
    }
    
    // Game state
    float survivalTime = 0.0f;
    bool gameActive = true;
    int enemiesKilled = 0;
    bool pausedForUpgrade = false;
    bool gameWon = false;
    bool gameLost = false;
    const float WIN_TIME = 180.0f; // 3 minutes to win

    // Main game loop
    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        
        // Update background music
        if (backgroundMusic.frameCount > 0) {
            UpdateMusicStream(backgroundMusic); // Keep music playing
        }
        
        // Update
        if (gameActive && player->IsAlive() && !pausedForUpgrade) {
            // Play background music if not playing
            if (backgroundMusic.frameCount > 0 && !IsMusicStreamPlaying(backgroundMusic)) {
                PlayMusicStream(backgroundMusic);
            }
            
            survivalTime += deltaTime;
            
            // Check win condition
            if (survivalTime >= WIN_TIME) {
                gameActive = false;
                gameWon = true;
            }
            
            player->Update(deltaTime);
  
            
            // Spawn enemies with progressive difficulty
            enemySpawnTimer += deltaTime;
            
            // Progressive spawn rate: gets faster as time passes
            float currentSpawnRate = enemySpawnRate;
            if (survivalTime > 120.0f) {  // After 2 minutes
                currentSpawnRate = 0.8f;  // Much faster
            } else if (survivalTime > 60.0f) {  // After 1 minute
                currentSpawnRate = 1.2f;  // Faster
            } else if (survivalTime > 30.0f) {  // After 30 seconds
                currentSpawnRate = 1.6f;  // Slower
            }
            
            if (enemySpawnTimer >= currentSpawnRate) {
                enemySpawnTimer = 0.0f;
                
                // Random spawn position at screen edges
                Vector2 spawnPos;
                int edge = GetRandomValue(0, 3);
                switch(edge) {
                    case 0: spawnPos = {(float)GetRandomValue(0, SCREEN_WIDTH), 0}; break;
                    case 1: spawnPos = {(float)SCREEN_WIDTH, (float)GetRandomValue(0, GAME_AREA_HEIGHT)}; break;
                    case 2: spawnPos = {(float)GetRandomValue(0, SCREEN_WIDTH), (float)GAME_AREA_HEIGHT}; break;
                    case 3: spawnPos = {0, (float)GetRandomValue(0, GAME_AREA_HEIGHT)}; break;
                }
                
                // Randomly select enemy type
                // Higher chance for runners, lower for tanks
                int typeRoll = GetRandomValue(1, 100);
                EnemyType enemyType;
                Texture2D* enemyTex = nullptr;
                
                if (typeRoll <= 20) {
                    enemyType = ENEMY_TANK;      // 20% chance
                    enemyTex = (enemyTankTexture.id > 0) ? &enemyTankTexture : nullptr;
                } else if (typeRoll <= 50) {
                    enemyType = ENEMY_BALANCED;  // 30% chance
                    enemyTex = (enemyBalancedTexture.id > 0) ? &enemyBalancedTexture : nullptr;
                } else {
                    enemyType = ENEMY_RUNNER;    // 50% chance
                    enemyTex = (enemyRunnerTexture.id > 0) ? &enemyRunnerTexture : nullptr;
                }
                
                enemies.push_back(new Enemy(spawnPos, enemyType, enemyTex));
            }
            
            // Update enemies
            for (Enemy* enemy : enemies) {
                if (enemy->active) {
                    enemy->Update(deltaTime, player->position);
                    
                    // Check collision with player
                    if (enemy->CheckCollision(player->position, player->radius)) {
                        player->TakeDamage(enemy->GetDamage());
                        if (hitSound.frameCount > 0) {
                            PlaySound(hitSound);
                        }
                        enemy->active = false; // Enemy dies on contact
                    }
                }
            }
            
            //Auto-attack with current weapon
            if (player->currentWeapon != nullptr) {
                player->currentWeapon->Update(deltaTime);
                
                if (player->currentWeapon->CanFire()) {
                    Enemy* target = player->currentWeapon->FindNearestEnemy(player->position, enemies);
                    if (target != nullptr) {
                        Projectile* projectile = player->currentWeapon->Fire(player->position, target, player->baseDamageMultiplier);
                        if (projectile != nullptr) {
                            projectiles.push_back(projectile);
                            player->currentWeapon->ResetFireTimer();
                        }
                    }
                }
            }
            
            // Update projectiles
            for (Projectile* proj : projectiles) {
                if (proj->active) {
                    proj->Update(deltaTime);
                    
                    // Check collision with enemies
                    for (Enemy* enemy : enemies) {
                        if (enemy->active && enemy->CheckCollision(proj->position, proj->radius)) {
                            enemy->TakeDamage(proj->damage);
                            proj->active = false;
                            
                            if (!enemy->active) {
                                enemiesKilled++;
                                if (enemyDeathSound.frameCount > 0) {
                                    PlaySound(enemyDeathSound);
                                }
                                
                                // Drop XP chest
                                ExperienceChest* xpDrop = new ExperienceChest(
                                    enemy->position,
                                    (xpChest.id > 0) ? &xpChest : nullptr,
                                    10  // XP amount
                                );
                                experienceChests.push_back(xpDrop);
                                
                                // Chance to drop weapon
                                if (GetRandomValue(0, 100) < 30) { // 30% drop chance
                                    WeaponType randomType = (WeaponType)(GetRandomValue(0, 2));
                                    
                                    // Select appropriate texture
                                    Texture2D* dropTexture = nullptr;
                                    switch(randomType) {
                                        case WEAPON_SWORD: dropTexture = &swordTexture; break;
                                        case WEAPON_BOW: dropTexture = &bowTexture; break;
                                        case WEAPON_STAFF: dropTexture = &staffTexture; break;
                                    }
                                    
                                    // Create weapon drop slightly offset from enemy position
                                    Vector2 weaponDropPos = {enemy->position.x + 30, enemy->position.y};
                                    weaponDrops.push_back(new WeaponDrop(weaponDropPos, randomType, dropTexture));
                                }
                            }
                            break;
                        }
                    }
                }
            }
            
            // Check weapon pickups
            for (WeaponDrop* drop : weaponDrops) {
                if (!drop->collected && drop->CheckCollision(player->position, player->radius)) {
                    // Determine which texture to use
                    Texture2D* weaponTex = nullptr;
                    switch(drop->type) {
                        case WEAPON_SWORD: weaponTex = &swordTexture; break;
                        case WEAPON_BOW: weaponTex = &bowTexture; break;
                        case WEAPON_STAFF: weaponTex = &staffTexture; break;
                    }
                    
                    // Replace current weapon with picked up weapon
                    player->SetWeapon(new Weapon(drop->type, weaponTex));
                    drop->collected = true;
                }
            };
            
            // Check XP chest pickups
            for (ExperienceChest* xpChest : experienceChests) {
                if (!xpChest->collected && xpChest->CheckCollision(player->position, player->radius)) {
                    if (player->AddExp(xpChest->xpAmount)) { // Leveled up if add exp returns true
                        pausedForUpgrade = true; // Pause for upgrade selection
                        if (levelUpSound.frameCount > 0) {
                            PlaySound(levelUpSound);
                        }
                    }
                    xpChest->collected = true;
                }
            }

        }
        
        // Check lose condition
        if (gameActive && !player->IsAlive()) {
            gameActive = false;
            gameLost = true;
        }

        // Draw
        BeginDrawing();
        
            ClearBackground(BLACK);
            
            // Draw background
            DrawBackground(&floorTexture);
            
            // Draw weapon drops
            for (WeaponDrop* drop : weaponDrops) {
                drop->Draw();
            }
            
            // Draw XP chests
            for (ExperienceChest* xpChest : experienceChests) {
                xpChest->Draw();
            }
            
            // Draw enemies
            for (Enemy* enemy : enemies) {
                enemy->Draw();
            }
            
            // Draw projectiles
            for (Projectile* proj : projectiles) {
                proj->Draw();
            }
            
            // Draw player
            if (player->IsAlive()) {
                player->Draw();
            }
            
            // Draw UI
            DrawGameUI(player, survivalTime);
            
            // Draw weapon indicator
            int weaponUIX = 10;
            int weaponUIY = 30;
            DrawText("Current Weapon:", weaponUIX, weaponUIY, 16, WHITE);
            if (player->currentWeapon != nullptr) {
                DrawText(player->currentWeapon->name.c_str(), weaponUIX, weaponUIY + 20, 16, player->currentWeapon->projectileColor);
            }
            
            // Draw stats
            string killsText = "Kills: " + to_string(enemiesKilled);
            DrawText(killsText.c_str(), SCREEN_WIDTH - 150, 30, 16, WHITE);
            
            string enemyCountText = "Enemies: " + to_string(enemies.size());
            DrawText(enemyCountText.c_str(), SCREEN_WIDTH - 150, 50, 14, YELLOW);
            
            // Draw upgrade screen if paused for level up
            if (pausedForUpgrade) {
                Vector2 mousePos = GetMousePosition(); // Get current mouse position for hover effects
                DrawUpgradeScreen((int)mousePos.x, (int)mousePos.y);
                
                // Handle upgrade selection
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    // Create invisible rectangles for each upgrade option to detect clicks
                    int boxWidth = 300;
                    int boxHeight = 150;
                    int spacing = 50;
                    int startX = SCREEN_WIDTH/2 - (boxWidth * 3 + spacing * 2) / 2;
                    int boxY = 320;
                    
                    Rectangle damageBox = {(float)startX, (float)boxY, (float)boxWidth, (float)boxHeight};
                    Rectangle healthBox = {(float)(startX + boxWidth + spacing), (float)boxY, (float)boxWidth, (float)boxHeight};
                    Rectangle speedBox = {(float)(startX + (boxWidth + spacing) * 2), (float)boxY, (float)boxWidth, (float)boxHeight};
                    
                    if (CheckCollisionPointRec(mousePos, damageBox)) {
                        player->ApplyUpgrade(UPGRADE_DAMAGE);
                        pausedForUpgrade = false;
                    }
                    else if (CheckCollisionPointRec(mousePos, healthBox)) {
                        player->ApplyUpgrade(UPGRADE_HEALTH);
                        pausedForUpgrade = false;
                    }
                    else if (CheckCollisionPointRec(mousePos, speedBox)) {
                        player->ApplyUpgrade(UPGRADE_SPEED);
                        pausedForUpgrade = false;
                    }
                }
            }
            
            // Draw end game screens
            if (gameWon) {
                // Play win sound once
                static bool winSoundPlayed = false;
                if (!winSoundPlayed && winSound.frameCount > 0) {
                    PlaySound(winSound);
                    winSoundPlayed = true;
                }
                
                // Semi-transparent overlay
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.6f));
                
                // Victory text
                const char* winText = "VICTORY!";
                int winWidth = MeasureText(winText, 80);
                DrawText(winText, SCREEN_WIDTH/2 - winWidth/2, SCREEN_HEIGHT/2 - 100, 80, GOLD);
                
                const char* subText = "You survived for 3 minutes!";
                int subWidth = MeasureText(subText, 30);
                DrawText(subText, SCREEN_WIDTH/2 - subWidth/2, SCREEN_HEIGHT/2 + 20, 30, WHITE);
                
                int minutes = (int)survivalTime / 60;
                int seconds = (int)survivalTime % 60;
                string timeText = "Time Survived: " + to_string(minutes) + "m " + to_string(seconds) + "s";
                int timeWidth = MeasureText(timeText.c_str(), 24);
                DrawText(timeText.c_str(), SCREEN_WIDTH/2 - timeWidth/2, SCREEN_HEIGHT/2 + 70, 24, SKYBLUE);

                string levelText = "Level Reached: " + to_string(player->level);
                int levelWidth = MeasureText(levelText.c_str(), 24);
                DrawText(levelText.c_str(), SCREEN_WIDTH/2 - levelWidth/2, SCREEN_HEIGHT/2 + 110, 24, GOLD);
                
                string killText = "Enemies Defeated: " + to_string(enemiesKilled);
                int killWidth = MeasureText(killText.c_str(), 24);
                DrawText(killText.c_str(), SCREEN_WIDTH/2 - killWidth/2, SCREEN_HEIGHT/2 + 150, 24, YELLOW);
            }
            
            if (gameLost) {
                // Semi-transparent overlay
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.6f));
                
                // Defeat text
                const char* loseText = "GAME OVER";
                int loseWidth = MeasureText(loseText, 80);
                DrawText(loseText, SCREEN_WIDTH/2 - loseWidth/2, SCREEN_HEIGHT/2 - 100, 80, RED);
                
                const char* subText = "You were defeated!";
                int subWidth = MeasureText(subText, 30);
                DrawText(subText, SCREEN_WIDTH/2 - subWidth/2, SCREEN_HEIGHT/2 + 20, 30, WHITE);
                
                int minutes = (int)survivalTime / 60;
                int seconds = (int)survivalTime % 60;
                string timeText = "Time Survived: " + to_string(minutes) + "m " + to_string(seconds) + "s";
                int timeWidth = MeasureText(timeText.c_str(), 24);
                DrawText(timeText.c_str(), SCREEN_WIDTH/2 - timeWidth/2, SCREEN_HEIGHT/2 + 70, 24, SKYBLUE);
                
                string levelText = "Level Reached: " + to_string(player->level);
                int levelWidth = MeasureText(levelText.c_str(), 24);
                DrawText(levelText.c_str(), SCREEN_WIDTH/2 - levelWidth/2, SCREEN_HEIGHT/2 + 110, 24, GOLD);
                
                string killText = "Enemies Defeated: " + to_string(enemiesKilled);
                int killWidth = MeasureText(killText.c_str(), 24);
                DrawText(killText.c_str(), SCREEN_WIDTH/2 - killWidth/2, SCREEN_HEIGHT/2 + 150, 24, YELLOW);
            }

            
        EndDrawing();
    }

    // De-Initialization
    delete player;
    
    // unload audio
    if (backgroundMusic.frameCount > 0) {
        StopMusicStream(backgroundMusic);
        UnloadMusicStream(backgroundMusic);
    }
    
    if (hitSound.frameCount > 0) UnloadSound(hitSound);
    if (levelUpSound.frameCount > 0) UnloadSound(levelUpSound);
    if (enemyDeathSound.frameCount > 0) UnloadSound(enemyDeathSound);
    if (winSound.frameCount > 0) UnloadSound(winSound);
    
    CloseAudioDevice();
    
    // Cleanup game entities
    for (Enemy* enemy : enemies) delete enemy;
    for (Projectile* proj : projectiles) delete proj;
    for (WeaponDrop* drop : weaponDrops) delete drop;
    for (ExperienceChest* xpChest : experienceChests) delete xpChest;
    
    enemies.clear();
    projectiles.clear();
    weaponDrops.clear();
    experienceChests.clear();
    
    // Unload textures if they were loaded
    if (floorTexture.id > 0) {
        UnloadTexture(floorTexture);
    }
    if (playerTexture.id > 0) {
        UnloadTexture(playerTexture);
    }
    if (swordTexture.id > 0) {
        UnloadTexture(swordTexture);
    }
    if (bowTexture.id > 0) {
        UnloadTexture(bowTexture);
    }
    if (staffTexture.id > 0) {
        UnloadTexture(staffTexture);
    }
    if (enemyTankTexture.id > 0) {
        UnloadTexture(enemyTankTexture);
    }
    if (enemyBalancedTexture.id > 0) {
        UnloadTexture(enemyBalancedTexture);
    }
    if (enemyRunnerTexture.id > 0) {
        UnloadTexture(enemyRunnerTexture);
    }
    if (xpChest.id > 0) {
        UnloadTexture(xpChest);
    }
    
    CloseWindow();
    
    return 0;
}