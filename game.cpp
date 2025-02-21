#include <Ogre.h>
#include <OIS.h>
#include <vector>
#include <memory>

// 全局变量（为了简化示例，未使用类封装）
Ogre::Root* root;
Ogre::RenderWindow* window;
Ogre::SceneManager* sceneMgr;
Ogre::Camera* camera;
Ogre::SceneNode* playerNode;
std::vector<Ogre::SceneNode*> enemies;
OIS::InputManager* inputMgr;
OIS::Keyboard* keyboard;
OIS::Mouse* mouse;

// 函数声明
void init();
void gameLoop();
void handleInput(float deltaTime);
void updateGame(float deltaTime);
void render();
void shoot();

// 主函数
int main() {
    init();
    gameLoop();
    return 0;
}

// 初始化游戏
void init() {
    // 初始化Ogre3D
    root = new Ogre::Root();
    if (!root->showConfigDialog()) {
        exit(-1); // 显示配置对话框，失败则退出
    }
    window = root->initialise(true, "射击类3D游戏");
    sceneMgr = root->createSceneManager();

    // 设置摄像机（第一人称视角）
    camera = sceneMgr->createCamera("PlayerCam");
    camera->setPosition(Ogre::Vector3(0, 10, 50));
    camera->lookAt(Ogre::Vector3(0, 0, 0));
    camera->setNearClipDistance(5);

    // 添加视口
    Ogre::Viewport* vp = window->addViewport(camera);
    vp->setBackgroundColour(Ogre::ColourValue(0.2, 0.2, 0.2)); // 灰色背景

    // 初始化输入系统
    size_t windowHnd = 0;
    window->getCustomAttribute("WINDOW", &windowHnd);
    inputMgr = OIS::InputManager::createInputSystem(windowHnd);
    keyboard = static_cast<OIS::Keyboard*>(inputMgr->createInputObject(OIS::OISKeyboard, false));
    mouse = static_cast<OIS::Mouse*>(inputMgr->createInputObject(OIS::OISMouse, false));
    mouse->setEventCallback(nullptr); // 未实现完整回调，简化处理

    // 创建玩家
    Ogre::Entity* playerEntity = sceneMgr->createEntity("Player", "player.mesh");
    playerNode = sceneMgr->getRootSceneNode()->createChildSceneNode("PlayerNode");
    playerNode->attachObject(playerEntity);
    playerNode->setPosition(0, 0, 0);

    // 创建敌人
    Ogre::Entity* enemyEntity = sceneMgr->createEntity("Enemy", "enemy.mesh");
    Ogre::SceneNode* enemyNode = sceneMgr->getRootSceneNode()->createChildSceneNode("EnemyNode");
    enemyNode->attachObject(enemyEntity);
    enemyNode->setPosition(20, 0, 20);
    enemies.push_back(enemyNode);

    // 添加灯光
    Ogre::Light* light = sceneMgr->createLight("MainLight");
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setDirection(Ogre::Vector3(-1, -1, 0));
}

// 游戏主循环
void gameLoop() {
    while (!window->isClosed()) {
        Ogre::WindowEventUtilities::messagePump();
        handleInput(0.016f); // 假设60FPS，deltaTime = 1/60
        updateGame(0.016f);
        render();
    }

    // 清理资源
    delete root;
    inputMgr->destroyInputObject(keyboard);
    inputMgr->destroyInputObject(mouse);
    OIS::InputManager::destroyInputSystem(inputMgr);
}

// 处理输入
void handleInput(float deltaTime) {
    keyboard->capture();
    mouse->capture();

    // 移动玩家
    float moveSpeed = 20.0f;
    Ogre::Vector3 moveDir(0, 0, 0);
    if (keyboard->isKeyDown(OIS::KC_W)) moveDir.z -= moveSpeed * deltaTime;
    if (keyboard->isKeyDown(OIS::KC_S)) moveDir.z += moveSpeed * deltaTime;
    if (keyboard->isKeyDown(OIS::KC_A)) moveDir.x -= moveSpeed * deltaTime;
    if (keyboard->isKeyDown(OIS::KC_D)) moveDir.x += moveSpeed * deltaTime;
    playerNode->translate(moveDir);

    // 鼠标控制视角
    const OIS::MouseState& ms = mouse->getMouseState();
    camera->yaw(Ogre::Degree(-ms.X.rel * 0.1f));
    camera->pitch(Ogre::Degree(-ms.Y.rel * 0.1f));

    // 左键射击
    if (ms.buttonDown(OIS::MB_Left)) {
        shoot();
    }
}

// 更新游戏逻辑
void updateGame(float deltaTime) {
    // 更新敌人AI：朝玩家移动
    float enemySpeed = 5.0f;
    for (auto& enemy : enemies) {
        Ogre::Vector3 direction = playerNode->getPosition() - enemy->getPosition();
        direction.normalise();
        enemy->translate(direction * enemySpeed * deltaTime);
    }
}

// 渲染画面
void render() {
    root->renderOneFrame();
}

// 射击机制
void shoot() {
    // 使用射线检测
    Ogre::Ray ray(camera->getPosition(), camera->getDirection());
    Ogre::RaySceneQuery* rayQuery = sceneMgr->createRayQuery(ray);
    rayQuery->setSortByDistance(true);
    Ogre::RaySceneQueryResult& result = rayQuery->execute();

    for (auto& it : result) {
        if (it.movable && it.movable->getName() == "Enemy") {
            // 击中敌人（此处可添加销毁逻辑）
            std::cout << "击中敌人！" << std::endl;
            break;
        }
    }
    sceneMgr->destroyQuery(rayQuery);
}
