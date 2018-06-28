
void setOrthographicProjection(int w, int h, int location)
{
    GLfloat projection[4][4];
    projection[0][0] = 2.0f / w;
    projection[0][1] = 0.0f;
    projection[0][2] = 0.0f;
    projection[0][3] = 0.0f;
    projection[1][0] = 0.0f;
    projection[1][1] = 2.0f / h;
    projection[1][2] = 0.0f;
    projection[1][3] = 0.0f;
    projection[2][0] = 0.0f;
    projection[2][1] = 0.0f;
    projection[2][2] = 0.0f;
    projection[2][3] = 0.0f;
    projection[3][0] = -1.0f;
    projection[3][1] = -1.0f;
    projection[3][2] = 0.0f;
    projection[3][3] = 1.0f;

    glUniformMatrix4fv(location, 1, GL_FALSE, (GLfloat *)projection);
}
