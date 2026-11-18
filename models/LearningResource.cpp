#include "LearningResource.h"

// Base LearningResource
LearningResource::LearningResource(int id, int skillId, const std::string& title, const std::string& url)
    : m_id(id), m_skillId(skillId), m_title(title), m_url(url), m_difficulty("Intermediate") {}

int LearningResource::getId() const {
    return m_id;
}

void LearningResource::setId(int id) {
    m_id = id;
}

int LearningResource::getSkillId() const {
    return m_skillId;
}

void LearningResource::setSkillId(int skillId) {
    m_skillId = skillId;
}

std::string LearningResource::getTitle() const {
    return m_title;
}

void LearningResource::setTitle(const std::string& title) {
    m_title = title;
}

std::string LearningResource::getUrl() const {
    return m_url;
}

void LearningResource::setUrl(const std::string& url) {
    m_url = url;
}

std::string LearningResource::getDifficulty() const {
    return m_difficulty;
}

void LearningResource::setDifficulty(const std::string& difficulty) {
    m_difficulty = difficulty;
}

// VideoResource
VideoResource::VideoResource(int id, int skillId, const std::string& title, const std::string& url,
                             int durationMinutes, const std::string& difficulty)
    : LearningResource(id, skillId, title, url), m_durationMinutes(durationMinutes) {
    m_difficulty = difficulty;
}

int VideoResource::getDurationMinutes() const {
    return m_durationMinutes;
}

void VideoResource::setDurationMinutes(int durationMinutes) {
    m_durationMinutes = durationMinutes;
}

std::string VideoResource::resourceType() const {
    return "Video";
}

std::string VideoResource::learningDescription() const {
    return "Video lecture (" + std::to_string(m_durationMinutes) + " mins, " + m_difficulty + "): " + m_title;
}

std::string VideoResource::getDuration() const {
    return std::to_string(m_durationMinutes) + " mins";
}

// ArticleResource
ArticleResource::ArticleResource(int id, int skillId, const std::string& title, const std::string& url,
                                 int readTimeMinutes, const std::string& difficulty)
    : LearningResource(id, skillId, title, url), m_readTimeMinutes(readTimeMinutes) {
    m_difficulty = difficulty;
}

int ArticleResource::getReadTimeMinutes() const {
    return m_readTimeMinutes;
}

void ArticleResource::setReadTimeMinutes(int readTimeMinutes) {
    m_readTimeMinutes = readTimeMinutes;
}

std::string ArticleResource::resourceType() const {
    return "Article";
}

std::string ArticleResource::learningDescription() const {
    return "Documentation / Tutorial article (" + std::to_string(m_readTimeMinutes) + " mins read, " + m_difficulty + "): " + m_title;
}

std::string ArticleResource::getDuration() const {
    return std::to_string(m_readTimeMinutes) + " mins read";
}

// PracticeResource
PracticeResource::PracticeResource(int id, int skillId, const std::string& title, const std::string& url,
                                   const std::string& platform, int exerciseCount, const std::string& difficulty)
    : LearningResource(id, skillId, title, url), m_platform(platform), m_exerciseCount(exerciseCount) {
    m_difficulty = difficulty;
}

std::string PracticeResource::getPlatform() const {
    return m_platform;
}

void PracticeResource::setPlatform(const std::string& platform) {
    m_platform = platform;
}

int PracticeResource::getExerciseCount() const {
    return m_exerciseCount;
}

void PracticeResource::setExerciseCount(int count) {
    m_exerciseCount = count;
}

std::string PracticeResource::resourceType() const {
    return "Practice";
}

std::string PracticeResource::learningDescription() const {
    return "Interactive coding lab on " + m_platform + " (" + std::to_string(m_exerciseCount) + " exercises, " + m_difficulty + "): " + m_title;
}

std::string PracticeResource::getDuration() const {
    return std::to_string(m_exerciseCount) + " exercises";
}
