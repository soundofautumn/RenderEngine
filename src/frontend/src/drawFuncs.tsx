import { client } from "./client";
import { IPoint } from "./types";

interface IDrawFuncParams {
  pointers: IPoint[];
  color?: string;
  algorithm?: number;
}

class DrawFunc {
  public readonly requiredPointers: number;
  public readonly apiEndpoint: string;
  public readonly drawingMethod: 'click' | 'drag';

  constructor(requiredPointers: number, apiEndpoint: string, drawingMethod: 'click' | 'drag' = 'drag') {
    this.requiredPointers = requiredPointers;
    this.apiEndpoint = apiEndpoint;
    this.drawingMethod = drawingMethod;
  }

  public draw(props: IDrawFuncParams): Promise<void> {
    const { pointers, color = '#ffffffff', algorithm = 0 } = props;
    return new Promise((resolve, reject) => {
      if (pointers.length < this.requiredPointers) {
        reject("Not enough pointers");
      }
      client("/engine/draw", {
        data: {
          [this.apiEndpoint]: {
            ...Object.fromEntries(pointers.map((pointer, index) => {
              return ['p' + (index + 1), {
                ...pointer,
              }];
            })),
            color: {
              r: parseInt(color.slice(1, 3), 16),
              g: parseInt(color.slice(3, 5), 16),
              b: parseInt(color.slice(5, 7), 16),
              a: parseInt(color.slice(7, 9), 16),
            },
            algorithm,
          }
        }
      }).then(() => {
        resolve();
      }).catch(e => {
        console.error(e);
        reject("Failed to draw");
      })
    });
  }
}

interface IDrawFunc { 
  name: string;
  drawFunc: DrawFunc;
}

const drawFuncs: IDrawFunc[] = []

drawFuncs.push({
  name: 'Click to Draw Line',
  drawFunc: new DrawFunc(2, 'Line', 'click'),
})

drawFuncs.push({
  name: 'Drag to Draw Line',
  drawFunc: new DrawFunc(2, 'Line', 'drag'),
})

export default drawFuncs;
