#include "cgl/interface.h"

#include <cgl/canvas.h>
extern CglCanvas *pcv;

////////////////////////////////////////////////////////////////
//Button class

//Constructor
CglButton::CglButton(float s, glm::vec2 c, glm::vec3 col, string texturePath){
  size    = s;
  center  = c;
  color   = col;
  mins    = glm::vec2(c.x - s/2, c.y - s/2);
  maxs    = glm::vec2(c.x + s/2, c.y + s/2);
  texture.loadPNG(texturePath);
}

void CglButton::updateTexture(string texturePath){
  texture.loadPNG(texturePath);
  display();
}

//Display
void CglButton::display(){
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glUseProgram(0);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texture.getID());

  float z = 1;//pcv->getScene()->getView()->m_znear;
  float ratio = pcv->getScene()->getView()->ratio;

  glBegin(GL_QUAD_STRIP);
    glColor3f(color.x, color.y, color.z);
    glTexCoord2f(0.0, 1.0);   glVertex3f(center.x - (size/2)/ratio, -center.y - size/2, z);
    glTexCoord2f(1.0, 1.0);   glVertex3f(center.x + (size/2)/ratio, -center.y - size/2, z);
    glTexCoord2f(0.0, 0.0);   glVertex3f(center.x - (size/2)/ratio, -center.y + size/2, z);
    glTexCoord2f(1.0, 0.0);   glVertex3f(center.x + (size/2)/ratio, -center.y + size/2, z);
  glEnd();

  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
}

void CglButton::activate(){
  if(pcv->profile.interface_type == CGL_BUTTONS_BASIC){
    if( rank == 0 ){
      pcv->profile.switch_theme();
      pcv->getInterface()->updateTextures();
    }
    if( rank == 1 ){
      pcv->profile.displayBottomGrid = !pcv->profile.displayBottomGrid;
    }
    if( rank == 2 ){
      pcv->profile.smooth = !pcv->profile.smooth;
    }
    if( rank == 3 ){
      for (unsigned int i = 0; i < pcv->getScene()->numObjects() ; i++)
        pcv->getScene()->getObject(i)->toogleMesh();
    }
    if( rank == 4 ){
      exit(1);
    }
  }
  else if(pcv->profile.interface_type == CGL_BUTTONS_EXTENDED){
    if( rank == 0 ){
      pcv->profile.switch_theme();
      pcv->getInterface()->updateTextures();
    }
    if( rank == 1 ){
      pcv->profile.switch_colors();
    }
    if( rank == 2 ){
      pcv->profile.displayBottomGrid = !pcv->profile.displayBottomGrid;
    }
    if( rank == 3 ){
      pcv->profile.smooth = !pcv->profile.smooth;
    }
    if( rank == 4 ){
      for (unsigned int i = 0; i < pcv->getScene()->numObjects() ; i++)
        pcv->getScene()->getObject(i)->toogleMesh();
    }
    if( rank == 5 ){
      pcv->profile.perspective = !pcv->profile.perspective;
    }
    if( rank == 6 ){
      std::vector<pCglObject> objectsToGroup;
      for(int i = 0 ; i < pcv->getScene()->numObjects() ; i++){
        if(pcv->getScene()->getObject(i)->isSelected()){
          objectsToGroup.push_back(pcv->getScene()->getObject(i));
        }
      }
      if(objectsToGroup.size()>1){
        pcv->getScene()->getGroupList()->push_back(new CglGroup(objectsToGroup));
      }
    }
    if( rank == 7 ){
      exit(1);
    }
  }
}



////////////////////////////////////////////////////////////////
//Interface class
void CglInterface::init_buttons(){
  CGL_BUTTONS interface = pcv->profile.interface_type;
  if(interface == CGL_BUTTONS_BASIC){
    nbButtons = 5;
    icons =  {"fill.png",
              "grid.png",
              "normals.png",
              "mesh.png",
              "exit.png"};
  }
  else if(interface == CGL_BUTTONS_EXTENDED){
    nbButtons = 8;
    icons =  {"fill.png",
              "color.png",
              "grid.png",
              "normals.png",
              "mesh.png",
              "cam.png",
              "lock.png",
              "exit.png"};
  }
};

string CglInterface::getIconsFolder(){
  CGL_THEME theme = pcv->profile.theme;
  string folder;
  if(theme == CGL_THEME_BLACK)
    folder = "White/";
  else if(theme == CGL_THEME_WHITE)
    folder = "Black/";
  else
    folder = "Color/";
  return folder;
}

void CglInterface::updateTextures(){
  folder = getIconsFolder();
  for(int i = 0 ; i < nbButtons ; i++){
    buttons[i]->updateTexture(pcv->profile.path + "icons/" + folder + icons[i]);
  }
}

void CglInterface::hover(int indButton){
  for(int i = 0 ; i < nbButtons ; i++){
    if(indButton==-1)
      hovered[i] = false;
    else if(i == indButton)
      hovered[i] = true;
    else
      hovered[i] = false;
  }
}


////////////////////////////////////////////////////////////////
//Linear interface

void CglLinearInterface::init(float off){
  init_buttons();
  hovered.clear();
  folder          = getIconsFolder();
  isMouseOnPanel  = false;
  float space     = 2.0 / (nbButtons + 1) ;

  for(int i = 0 ; i < nbButtons ; i++){
    pCglButton b = new CglButton(0.15,
                                glm::vec2(off, (i+1) * space - 1),
                                glm::vec3(1,1,1),
                                pcv->profile.path + "icons/" + folder + icons[i]);
    b->setRank(i);
    buttons.push_back(b);
    hovered.push_back(false);
  }
  offset = off;
}

void CglLinearInterface::display(){
  glm::vec2 pos = pcv->getMouse()->getCursorPosition();
  int panelBorder = (1 + offset - (1-offset)) * pcv->getScene()->getView()->width/2;
  isMouseOnPanel = ((pos.x > panelBorder) ? true:false);
  active = isMouseOnPanel;

  if(active){
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    for(int i = 0 ; i < nbButtons ; i++){
      glBegin(GL_POLYGON);
      glm::vec4 col = ((hovered[i])?glm::vec4(pcv->profile.sele_color,0.8):glm::vec4(1,1,1,0.5));
      glColor4f(col[0], col[1], col[2], col[3]);
      glVertex2f(1-(2-2*offset), 1 - 1.0f/(nbButtons+1) - i*2.0f/(nbButtons+1) );
      glVertex2f(1             , 1 - 1.0f/(nbButtons+1) - i*2.0f/(nbButtons+1));
      glVertex2f(1             , 1 - 1.0f/(nbButtons+1) - (i+1)*2.0f/(nbButtons+1));
      glVertex2f(1-(2-2*offset), 1 - 1.0f/(nbButtons+1) - (i+1)*2.0f/(nbButtons+1) );
      glEnd();
    }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    for(int i = 0 ; i < nbButtons ; i++){
      buttons[i]->display();
    }
  }
}


////////////////////////////////////////////////////////////////
//Radial interface

void CglRadialInterface::init(glm::vec2 cen, float rad){
  init_buttons();
  folder            = getIconsFolder();
  float ratio       = pcv->getScene()->getView()->ratio;
  active            = true;
  isMouseOnPanel    = true;
  center            = cen;
  buttons.clear();
  hovered.clear();

  for(int i = 0 ; i < nbButtons ; i++){
    float angle    = 3.14159 / 2.0  +  i * (2.0 * 3.14159)/nbButtons;
    glm::vec2  pos = rad * glm::vec2(cos(angle), sin(angle));
    pCglButton b   = new CglButton(0.15,
                                  glm::vec2(center.x + pos.x/ratio, center.y - pos.y),
                                  glm::vec3(1,1,1),
                                  pcv->profile.path + "icons/" + folder + icons[i]);
    b->setRank(i);
    buttons.push_back(b);
    hovered.push_back(false);
  }
  radius = rad;
  updateTextures();
}

void CglRadialInterface::display(){
  glm::vec2 mousePos = pcv->getMouse()->getCursorPosition();
  float w            = pcv->getScene()->getView()->width;
  float h            = pcv->getScene()->getView()->height;
  glm::vec2 pos      = glm::vec2( (mousePos.x - w/2)/(w/2), (mousePos.y - h/2)/(h/2) );
  float distance     = glm::length(center - pos);
  isMouseOnPanel     = ((distance < radius + 0.15) ? true:false);

  if (active){
    displayWheel(7);
    for(int i = 0 ; i < nbButtons ; i++){
      buttons[i]->display();
    }
  }
}

void CglRadialInterface::displayWheel(int step){
  int nb            = nbButtons * step;
  float ratio       = pcv->getScene()->getView()->ratio;

  glEnable(GL_BLEND);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  glBegin(GL_QUAD_STRIP);
  glDisable(GL_DEPTH_TEST);

  float innerRad = radius - 0.12;
  float outerRad = radius + 0.12;
  glm::vec4 col;

  for(int i = 0 ; i < nbButtons ; i++){
    col = ((hovered[i])?glm::vec4(pcv->profile.sele_color,0.8):glm::vec4(1,1,1,0.5));
    for(int j = -step/2 ; j < step/2 +1 ; j++){
      if((j==-step/2))
        glColor4f(1,1,1,0.5);
      else
        glColor4f(col[0], col[1], col[2], col[3]);

      int k = i*step + j;

      float angle = 3.14159 / 2.0  - 3.14159/nb  +  k     * (2.0 * 3.14159)/nb;//
      glm::vec2 pos = glm::vec2(cos(angle), sin(angle));

      glVertex2f(center.x + innerRad * pos.x /ratio,     -center.y + innerRad * pos.y);
      glVertex2f(center.x + outerRad * pos.x /ratio,     -center.y + outerRad * pos.y);
    }
  }

  int k = (nbButtons - 1) * step + step/2 + 1;
  float angle = 3.14159 / 2.0  - 3.14159/nb  +  k     * (2.0 * 3.14159)/nb;
  glm::vec2  pos     = glm::vec2(cos(angle), sin(angle));
  glColor4f(1,1,1,0.5);
  glVertex2f(center.x + innerRad * pos.x /ratio,     -center.y + innerRad * pos.y);
  glVertex2f(center.x + outerRad * pos.x /ratio,     -center.y + outerRad * pos.y);


  glEnd();
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
}
