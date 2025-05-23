#include "LevelSelect.h"

LevelSelectMenu::LevelSelectMenu(Windowing& window, TTF& textRenderer, Controller& controller, GameState& gameState)
    : window(window), textRenderer(textRenderer), controller(controller), gameState(gameState),
    permanentButton(0, 0, 0, 0, glm::vec3(0, 0, 0)),
    normalButton(0, 0, 0, 0, glm::vec3(0, 0, 0)),
    backButton(0, 0, 0, 0, glm::vec3(0, 0, 0)) {
    initializeUIText();
    compileShaders();
    backgroundRenderer = new BackgroundRenderer("project/assets/background/selectGameMode.jpg", shader);
    backgroundImageTexture = loadTexture("project/assets/background/longTrails.png");
    backgroundImageTexture1 = loadTexture("project/assets/background/shortTrails.png");
}

LevelSelectMenu::~LevelSelectMenu() {
    if (backgroundImageTexture != 0) {
        glDeleteTextures(1, &backgroundImageTexture); 
    }

    if (backgroundImageTexture1 != 0) {
        glDeleteTextures(1, &backgroundImageTexture1);
    }
    delete backgroundRenderer;
    backgroundRenderer = nullptr;
    uiText.clear();
    delete shader;
    shader = nullptr;

    //delete boxRenderer;
}

int LevelSelectMenu::displayMenuLevel() {
    glfwSetInputMode(window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    /*if (!audioInitialized) {
        audio.init();
        audio.startLevelMusic();
        audioInitialized = true;
    }
    audio.startLevelMusic();
    */
    int selectedLevel = 0;


    while (selectedLevel == 0 && !window.shouldClose()) {
        window.clear();
        renderMenu();
        window.swapBuffer();
        glfwPollEvents();
        

        handleKeyboardInput(selectedLevel);
        handleControllerInput(selectedLevel);

        if (glfwGetMouseButton(window.getGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window.getGLFWwindow(), &xpos, &ypos);

            if (permanentButton.isClicked(xpos, ypos)) {
                selectedLevel = 1; // Permanent
                //audio.stopMusic();
            }
            else if (normalButton.isClicked(xpos, ypos)) {
                selectedLevel = 2; // Normal
                //audio.stopMusic();
            }
            else if (backButton.isClicked(xpos, ypos)) {
                selectedLevel = -1;
                //audio.stopMusic();
            }
        }
    }

    return selectedLevel;
}

void LevelSelectMenu::renderMenu() {
    glDisable(GL_DEPTH_TEST);
    backgroundRenderer->draw();
    glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);

    float buttonY = 0.85f * windowHeight;
    float buttonHeight = 0.0625f * windowHeight;
    float buttonWidth = 0.2f * windowWidth;
    float spacing = 0.05f * windowWidth;
    float totalWidth = 3 * buttonWidth + 2 * spacing;
    float startX = (windowWidth - totalWidth) / 2;

    permanentButton = Button(startX, buttonY, buttonWidth, buttonHeight, glm::vec3(1, 0, 0));
    backButton = Button(startX + buttonWidth + spacing, buttonY, buttonWidth, buttonHeight, glm::vec3(1, 0, 0));
    normalButton = Button(startX + 2 * (buttonWidth + spacing), buttonY, buttonWidth, buttonHeight, glm::vec3(1, 0, 0));


    permanentButton.setColor(currentSelection == 0 ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0));
    backButton.setColor(currentSelection == 1 ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0));
    normalButton.setColor(currentSelection == 2 ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0));


    permanentButton.draw(shader, windowWidth, windowHeight);
    normalButton.draw(shader, windowWidth, windowHeight);
    backButton.draw(shader, windowWidth, windowHeight);

    renderText(uiText);
    float boxWidth = 0.6f;
    float boxHeight = 1.2f;
    float y = -0.6f;
    float spacingB = 0.4f;

    float totalWidthB = 2 * boxWidth + spacingB;
    float leftX = -totalWidthB / 2.0f;
    float rightX = leftX + boxWidth + spacingB;

    float normalAlpha = 0.70f;
    float highlightAlpha = 0.3f;

    bool isPermanentSelected = currentSelection == 0;
    bool isNormalSelected = currentSelection == 2;

    float imageWidth = 0.4f;
    float imageHeight = 0.5f;

    //centering a div but worse
    float leftImageX = leftX + (boxWidth - imageWidth) / 2.0f;
    float rightImageX = rightX + (boxWidth - imageWidth) / 2.0f;
    float imageY = y + (boxHeight - imageHeight - 0.5f) / 2.0f;

    renderImage(backgroundImageTexture, leftImageX, imageY, imageWidth, imageHeight);
    renderImage(backgroundImageTexture1, rightImageX, imageY, imageWidth, imageHeight);
    
    boxRenderer.draw(leftX, y, boxWidth, boxHeight, isPermanentSelected ? highlightAlpha : normalAlpha);
    boxRenderer.draw(rightX, y, boxWidth, boxHeight, isNormalSelected ? highlightAlpha : normalAlpha);

}
void LevelSelectMenu::initializeUIText() {
    glfwGetWindowSize(window.getGLFWwindow(), &windowWidth, &windowHeight);

    float buttonHeight = 0.1 * windowHeight;
    float buttonWidth = 0.2f * windowWidth;
    float spacing = 0.05f * windowWidth;
    float totalWidth = 3 * buttonWidth + 2 * spacing;
    float shiftRight = 0.1f * windowWidth;
    float startX = (windowWidth - totalWidth) / 2 + shiftRight;
    float baseScale = std::min(windowWidth, windowHeight) * 0.00085f;

    uiText.clear();
    uiText.push_back(Text("Permanent", startX, buttonHeight, baseScale, glm::vec3(1, 1, 1)));
    uiText.push_back(Text("Back", startX + buttonWidth + spacing, buttonHeight, baseScale, glm::vec3(1, 1, 1)));
    uiText.push_back(Text("Normal", startX + 2 * (buttonWidth + spacing), buttonHeight, baseScale, glm::vec3(1, 1, 1)));

    float descScale = std::min(windowWidth, windowHeight) * 0.00050f;

    float permanentDescX = 0.25f * windowWidth;
    float normalDescX = 0.75f * windowWidth; 
    float initialDescY = 0.6525f * windowHeight;
    float descSpacing = 0.04f * windowHeight;

    uiText.push_back(Text("Normal: Trails disappear over time", normalDescX, initialDescY + 3 * descSpacing, descScale, glm::vec3(1, 1, 1)));
    uiText.push_back(Text("The **Normal Path** is fleeting,", normalDescX, initialDescY + 2 * descSpacing, descScale, glm::vec3(1, 1, 1)));
    uiText.push_back(Text("where trails fade with time", normalDescX, initialDescY + descSpacing, descScale, glm::vec3(1, 1, 1)));
    uiText.push_back(Text("and choices are left behind.", normalDescX, initialDescY, descScale, glm::vec3(1, 1, 1)));

    uiText.push_back(Text("Permanent: Trails stay forever", permanentDescX, initialDescY + 3 * descSpacing, descScale, glm::vec3(1, 1, 1)));
    uiText.push_back(Text("In the **Permanent Path**,", permanentDescX, initialDescY + 2 * descSpacing, descScale, glm::vec3(1, 1, 1)));
    uiText.push_back(Text("your presence lingers,", permanentDescX, initialDescY + descSpacing, descScale, glm::vec3(1, 1, 1)));
    uiText.push_back(Text("leaving a mark that lasts forever.", permanentDescX, initialDescY, descScale, glm::vec3(1, 1, 1)));
}


void LevelSelectMenu::compileShaders() {
    shader = new Shader("LevelSelectShader", "project/assets/shaders/mainMenuShader.vert", "project/assets/shaders/mainMenuShader.frag");
}

void LevelSelectMenu::renderText(const std::vector<Text>& renderingText) {
    for (const auto& text : renderingText) {
        textRenderer.render(text.getTextToRender(), text.getX(), text.getY(), text.getScale(), text.getColor());
    }
}

void LevelSelectMenu::handleKeyboardInput(int& selectedLevel) {
    static bool keyLeftReleased = false;
    static bool keyRightReleased = false;
    static bool keyEnterReleased = false;

    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_LEFT) == GLFW_RELEASE && keyLeftReleased) {
        currentSelection = (currentSelection - 1 + 3) % 3;
        keyLeftReleased = false;
    }
    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_LEFT) == GLFW_PRESS) {
        keyLeftReleased = true;
    }

    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_RIGHT) == GLFW_RELEASE && keyRightReleased) {
        currentSelection = (currentSelection + 1) % 3;
        keyRightReleased = false;
    }
    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_RIGHT) == GLFW_PRESS) {
        keyRightReleased = true;
    }

    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_ENTER) == GLFW_RELEASE && keyEnterReleased) {
        if (currentSelection == 1) { // BACK is now index 1
            selectedLevel = -1;
        }
        else if (currentSelection == 2) { // NORMAL is now index 2
            selectedLevel = 2;
        }
        else if (currentSelection == 0) { // PERMANENT
            selectedLevel = 1;
        }
        keyEnterReleased = false;
    }

    if (glfwGetKey(window.getGLFWwindow(), GLFW_KEY_ENTER) == GLFW_PRESS) {
        keyEnterReleased = true;
    }
}

void LevelSelectMenu::handleControllerInput(int& selectedLevel) {
    static bool dpadLeftReleased = false;
    static bool dpadRightReleased = false;
    static bool aButtonReleased = false;

    if (!controller.isConnected()) return;

    if (controller.isButtonReleased(XINPUT_GAMEPAD_DPAD_LEFT) && dpadLeftReleased) {
        currentSelection = (currentSelection - 1 + 3) % 3;
        dpadLeftReleased = false;
    }
    if (controller.isButtonPressed(XINPUT_GAMEPAD_DPAD_LEFT)) {
        dpadLeftReleased = true;
    }

    if (controller.isButtonReleased(XINPUT_GAMEPAD_DPAD_RIGHT) && dpadRightReleased) {
        currentSelection = (currentSelection + 1) % 3;
        dpadRightReleased = false;
    }
    if (controller.isButtonPressed(XINPUT_GAMEPAD_DPAD_RIGHT)) {
        dpadRightReleased = true;
    }

    if (controller.isButtonReleased(XINPUT_GAMEPAD_A) && aButtonReleased) {
        if (currentSelection == 1) { // BACK is now index 1
            selectedLevel = -1;
        }
        else if (currentSelection == 2) { // NORMAL is now index 2
            selectedLevel = 2;
        }
        else if (currentSelection == 0) { // PERMANENT
            selectedLevel = 1;
        }
        aButtonReleased = false;
    }

    if (controller.isButtonPressed(XINPUT_GAMEPAD_A)) {
        aButtonReleased = true;
    }
}

GLuint LevelSelectMenu::loadTexture(const char* filepath) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filepath, &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "Failed to load texture" << std::endl;
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    stbi_set_flip_vertically_on_load(false);
    return textureID;
}

void LevelSelectMenu::renderImage(GLuint textureID, float x, float y, float width, float height) {
    glBindTexture(GL_TEXTURE_2D, textureID);

    shader->use();
    shader->setBool("useTexture", true); 
    shader->setBool("useSolidColor", false);

    float vertices[] = {
        x, y, 0.0f, 0.0f, 1.0f,
        x + width, y, 1.0f, 0.0f, 1.0f,
        x + width, y + height, 1.0f, 1.0f, 1.0f,

        x, y, 0.0f, 0.0f, 1.0f,
        x + width, y + height, 1.0f, 1.0f, 1.0f,
        x, y + height, 0.0f, 1.0f, 1.0f,
    };

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}
